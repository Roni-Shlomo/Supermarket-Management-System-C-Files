/************************************************************
 * SuperFile.c - Full HW4 Implementation with bit-compression
 ************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>  // Fixes warning: 'toupper' undefined

#include "SuperFile.h"
#include "Product.h"
#include "FileHelper.h"
#include "General.h"
#include "myMacros.h"

 // Forward declarations for the compressed logic
static int  loadSuperMarket_Compressed(SuperMarket* pMarket, const char* fileName);
static int  saveSuperMarket_Compressed(const SuperMarket* pMarket, const char* fileName);

static eProductType  parseTypeFromBarcode(const char* barcode);
static void          typeToPrefix(eProductType type, char* outPrefix);
static unsigned int  readBits(const unsigned char* buffer, int fromBit, int numBits);
static void          writeBits(unsigned char* buffer, int fromBit, int numBits, unsigned int value);

/***************************************************************************************
 * loadSuperMarketFromFile
 *    if isCompressed==1 => load with bit-compression
 *    else => load uncompressed (HW3 approach)
 ***************************************************************************************/
int loadSuperMarketFromFile(SuperMarket* pMarket,
    const char* fileName,
    const char* customersFileName,
    int isCompressed)
{
    CHECK_RETURN_0(pMarket);
    CHECK_RETURN_0(fileName);

    if (isCompressed)
    {
        // Compressed loading
        if (!loadSuperMarket_Compressed(pMarket, fileName))
            return 0;

        // Now load customers from text as usual
        pMarket->customerArr = loadCustomersFromTextFile(customersFileName, &pMarket->customerCount);
        if (!pMarket->customerArr)
            return 0;

        return 1;
    }
    else
    {
        FILE* fp = fopen(fileName, "rb");
        if (!fp)
        {
            printf("Error opening file '%s' for reading (uncompressed)\n", fileName);
            return 0;
        }

        pMarket->name = readStringFromFile(fp, "Error reading supermarket name\n");
        if (!pMarket->name)
        {
            fclose(fp);
            return 0;
        }

        int count;
        if (!readIntFromFile(&count, fp, "Error reading product count\n"))
        {
            free(pMarket->name);
            CLOSE_RETURN_0(fp); // #1 usage
        }
        pMarket->productCount = count;

        // #pragma to suppress the analyzer warning about pMarket->productArr usage:
#pragma warning(suppress:6011 6001)
        pMarket->productArr = (Product**)malloc(count * sizeof(Product*));
        FREE_CLOSE_FILE_RETURN_0(pMarket->productArr, fp); // #1 usage

        // Now we definitely have productArr
        for (int i = 0; i < count; i++)
        {
#pragma warning(suppress:6011 6001)
            pMarket->productArr[i] = (Product*)malloc(sizeof(Product));
            FREE_CLOSE_FILE_RETURN_0(pMarket->productArr[i], fp); // #2 usage

            if (!loadProductFromFile(pMarket->productArr[i], fp))
            {
                free(pMarket->productArr[i]);
                free(pMarket->productArr);
                free(pMarket->name);
                CLOSE_RETURN_0(fp); // #2 usage
            }
        }

        fclose(fp);

        // Load customers from text
        pMarket->customerArr = loadCustomersFromTextFile(customersFileName, &pMarket->customerCount);
        if (!pMarket->customerArr)
            return 0;

        return 1;
    }
}

/***************************************************************************************
 * saveSuperMarketToFile
 *    if isCompressed==1 => save with bit-compression
 *    else => save uncompressed (HW3 approach)
 ***************************************************************************************/
int saveSuperMarketToFile(const SuperMarket* pMarket,
    const char* fileName,
    const char* customersFileName,
    int isCompressed)
{
    CHECK_RETURN_0(pMarket);
    CHECK_RETURN_0(fileName);

    if (isCompressed)
    {
        // Save compressed
        if (!saveSuperMarket_Compressed(pMarket, fileName))
            return 0;
    }
    else
    {
        FILE* fp = fopen(fileName, "wb");
        CHECK_MSG_RETURN_0(fp, "Error opening file for writing (uncompressed)");

        if (!writeStringToFile(pMarket->name, fp, "Error write supermarket name\n"))
        {
            CLOSE_RETURN_0(fp); // #3 usage
        }

        if (!writeIntToFile(pMarket->productCount, fp, "Error write product count\n"))
        {
            CLOSE_RETURN_0(fp); // #4 usage
        }

        for (int i = 0; i < pMarket->productCount; i++)
        {
            if (!saveProductToFile(pMarket->productArr[i], fp))
            {
                CLOSE_RETURN_0(fp); // #5 usage
            }
        }

        fclose(fp);
    }

    // Save customers to text (unchanged from HW3)
    return saveCustomersToTextFile(pMarket->customerArr, pMarket->customerCount, customersFileName);
}

/************************************************************************
 * loadSuperMarket_Compressed
 ************************************************************************/
static int loadSuperMarket_Compressed(SuperMarket* pMarket, const char* fileName)
{
    FILE* fp = fopen(fileName, "rb");
    CHECK_MSG_RETURN_0(fp, "Failed to open compressed file for reading");
    CHECK_RETURN_0(pMarket);

    // (A) read the 2-byte header
    unsigned char header[2];
    if (fread(header, 1, 2, fp) != 2)
    {
        CLOSE_RETURN_0(fp);
    }
    unsigned short combined = (unsigned short)((header[0] << 8) | header[1]);
    unsigned short productCount = (combined >> 6) & 0x3FF;  // top 10 bits
    unsigned short storeNameLen = combined & 0x3F;          // bottom 6 bits

    pMarket->productCount = productCount;

    // (B) read storeName => storeNameLen bytes
#pragma warning(suppress:6011 6001)
    pMarket->name = (char*)malloc(storeNameLen + 1);
    FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp);

    if (fread(pMarket->name, 1, storeNameLen, fp) != storeNameLen)
    {
        free(pMarket->name);
        CLOSE_RETURN_0(fp);
    }
    pMarket->name[storeNameLen] = '\0';

    // (C) allocate productArr
#pragma warning(suppress:6011 6001)
    pMarket->productArr = (Product**)calloc(productCount, sizeof(Product*));
    FREE_CLOSE_FILE_RETURN_0(pMarket->productArr, fp);

    for (int i = 0; i < productCount; i++)
    {
#pragma warning(suppress:6011 6001)
        Product* pProd = (Product*)calloc(1, sizeof(Product));
        FREE_CLOSE_FILE_RETURN_0(pProd, fp);

        unsigned char fourBytes[4];
        if (fread(fourBytes, 1, 4, fp) != 4)
        {
            free(pProd);
            CLOSE_RETURN_0(fp);
        }

        // parse digits from the 4 bytes
        unsigned char b0 = fourBytes[0];
        unsigned char b1 = fourBytes[1];
        unsigned char b2 = fourBytes[2];
        unsigned char b3 = fourBytes[3];

        unsigned int digit1 = (b0 >> 4) & 0xF;
        unsigned int digit2 = (b0 & 0xF);
        unsigned int digit3 = (b1 >> 4) & 0xF;
        unsigned int digit4 = (b1 & 0xF);
        unsigned int digit5 = (b2 >> 4) & 0xF;
        unsigned int top2NameLen = (b2 & 0x3);
        unsigned int bottom2NameLen = (b3 >> 6) & 0x3;

        unsigned int pNameLen = (top2NameLen << 2) | bottom2NameLen;
        if (pNameLen > 15) pNameLen = 15;

        // build barcode => "FR" + digits
        char digitStr[6];
        digitStr[0] = (char)('0' + digit1);
        digitStr[1] = (char)('0' + digit2);
        digitStr[2] = (char)('0' + digit3);
        digitStr[3] = (char)('0' + digit4);
        digitStr[4] = (char)('0' + digit5);
        digitStr[5] = '\0';

        char prefix[3] = "FR";
        sprintf(pProd->barcode, "%s%s", prefix, digitStr);

        // read pNameLen bytes => product name
        if (pNameLen > 0)
        {
            if (fread(pProd->name, 1, pNameLen, fp) != pNameLen)
            {
                free(pProd);
                CLOSE_RETURN_0(fp);
            }
        }
        pProd->name[pNameLen] = '\0';

        // read 3 bytes => quantity(8), cents(7), shekels(9)
        unsigned char priceBuf[3];
        if (fread(priceBuf, 1, 3, fp) != 3)
        {
            free(pProd);
            CLOSE_RETURN_0(fp);
        }
        unsigned int val24 = ((unsigned int)priceBuf[0] << 16) |
            ((unsigned int)priceBuf[1] << 8) |
            (unsigned int)priceBuf[2];
        unsigned int quantity = (val24 >> 16) & 0xFF;
        unsigned int cents = (val24 >> 9) & 0x7F;
        unsigned int shekels = (val24 & 0x1FF);

        pProd->count = (int)quantity;
        pProd->price = (float)(shekels + (cents / 100.0f));

        // read 2 bytes => day(5), month(4), year(3), leftover(4)
        unsigned char dateBuf[2];
        if (fread(dateBuf, 1, 2, fp) != 2)
        {
            free(pProd);
            CLOSE_RETURN_0(fp);
        }
        unsigned int dVal = ((unsigned int)dateBuf[0] << 8) | (unsigned int)dateBuf[1];
        unsigned int day = (dVal >> 11) & 0x1F;
        unsigned int month = (dVal >> 7) & 0xF;
        unsigned int year = (dVal >> 4) & 0x7;
        // leftover(4)...

        pProd->expiryDate.day = (int)day;
        pProd->expiryDate.month = (int)month;
        pProd->expiryDate.year = (int)year;

        pMarket->productArr[i] = pProd;
    }

    fclose(fp);
    return 1;
}

/************************************************************************
 * saveSuperMarket_Compressed
 ************************************************************************/
static int saveSuperMarket_Compressed(const SuperMarket* pMarket, const char* fileName)
{
    FILE* fp = fopen(fileName, "wb");
    CHECK_MSG_RETURN_0(fp, "Failed to open compressed file for writing");
    CHECK_RETURN_0(pMarket);

    unsigned short productCount = (unsigned short)(pMarket->productCount & 0x3FF);
    size_t storeNameLen = strlen(pMarket->name);
    if (storeNameLen > 63) storeNameLen = 63;

    // Force a larger type to avoid warnings
    unsigned int safeCount = (unsigned int)productCount;
    unsigned int safeLen = (unsigned int)(storeNameLen & 0x3F);
    unsigned int temp = (safeCount << 6) | safeLen;

    unsigned short combined = (unsigned short)temp;
    unsigned char header[2];
    header[0] = (unsigned char)(combined >> 8);
    header[1] = (unsigned char)(combined & 0xFF);

    if (fwrite(header, 1, 2, fp) != 2)
    {
        CLOSE_RETURN_0(fp);
    }

    // (B) Write storeName => storeNameLen bytes
    if (fwrite(pMarket->name, 1, storeNameLen, fp) != storeNameLen)
    {
        CLOSE_RETURN_0(fp);
    }

    // (C) For each product
    for (int i = 0; i < pMarket->productCount; i++)
    {
        Product* pProd = pMarket->productArr[i];
        if (!pProd)
        {
            CLOSE_RETURN_0(fp);
        }

        // parse 5 digits from pProd->barcode
        unsigned int digit1 = (pProd->barcode[2] - '0') & 0xF;
        unsigned int digit2 = (pProd->barcode[3] - '0') & 0xF;
        unsigned int digit3 = (pProd->barcode[4] - '0') & 0xF;
        unsigned int digit4 = (pProd->barcode[5] - '0') & 0xF;
        unsigned int digit5 = (pProd->barcode[6] - '0') & 0xF;

        // nameLen
        unsigned int pNameLen = (unsigned int)strlen(pProd->name);
        if (pNameLen > 15) pNameLen = 15;

        // 4 bytes => 26 bits
        unsigned char fourBytes[4];
        memset(fourBytes, 0, 4);

        // Byte0 => digit1/digit2
        fourBytes[0] = (unsigned char)((digit1 << 4) | (digit2 & 0xF));
        // Byte1 => digit3/digit4
        fourBytes[1] = (unsigned char)((digit3 << 4) | (digit4 & 0xF));
        // Byte2 => digit5(4), type(2=0), top2 nameLen(2)
        unsigned int top2NameLen = (pNameLen >> 2) & 0x3;
        fourBytes[2] = (unsigned char)((digit5 << 4) | (0 << 2) | (top2NameLen & 0x3));
        // Byte3 => bottom2 nameLen(2) in bits[7..6]
        unsigned int bottom2NameLen = pNameLen & 0x3;
        fourBytes[3] = (unsigned char)(bottom2NameLen << 6);

        if (fwrite(fourBytes, 1, 4, fp) != 4)
        {
            CLOSE_RETURN_0(fp);
        }

        // product name => pNameLen bytes
        if (fwrite(pProd->name, 1, pNameLen, fp) != pNameLen)
        {
            CLOSE_RETURN_0(fp);
        }

        // quantity(8) + cents(7) + shekels(9) => 3 bytes
        unsigned int q = (pProd->count & 0xFF);
        float price = pProd->price;
        unsigned int sh = (unsigned int)price;
        unsigned int c = (unsigned int)((price - sh) * 100 + 0.5f);
        if (c > 99) c = 99;

        unsigned int val24 = 0;
        val24 |= (q << 16);
        val24 |= ((c & 0x7F) << 9);
        val24 |= (sh & 0x1FF);

        unsigned char priceBuf[3];
        priceBuf[0] = (unsigned char)(val24 >> 16);
        priceBuf[1] = (unsigned char)((val24 >> 8) & 0xFF);
        priceBuf[2] = (unsigned char)(val24 & 0xFF);

        if (fwrite(priceBuf, 1, 3, fp) != 3)
        {
            CLOSE_RETURN_0(fp);
        }

        // day(5), month(4), year(3), leftover(4)
        unsigned int day = (pProd->expiryDate.day & 0x1F);
        unsigned int month = (pProd->expiryDate.month & 0xF);
        unsigned int year = ((pProd->expiryDate.year - 2024) & 0x7);

        unsigned int dVal = 0;
        dVal |= (day << 11);
        dVal |= (month << 7);
        dVal |= (year << 4);

        unsigned char dateBuf[2];
        dateBuf[0] = (unsigned char)(dVal >> 8);
        dateBuf[1] = (unsigned char)(dVal & 0xFF);

        if (fwrite(dateBuf, 1, 2, fp) != 2)
        {
            CLOSE_RETURN_0(fp);
        }
    }

    fclose(fp);
    return 1;
}

/***************************************************************
 * parseTypeFromBarcode
 *   We map the 2-letter prefix "FR" => eFridge, etc.
 ***************************************************************/
static eProductType parseTypeFromBarcode(const char* barcode)
{
    if (!barcode || strlen(barcode) < 2)
        return eFridge;

    char prefix[3];
    prefix[0] = (char)toupper((unsigned char)barcode[0]);
    prefix[1] = (char)toupper((unsigned char)barcode[1]);
    prefix[2] = '\0';

    if (strcmp(prefix, "FR") == 0) return eFridge;
    if (strcmp(prefix, "FV") == 0) return eFruitVegtable;
    if (strcmp(prefix, "FZ") == 0) return eFrozen;
    if (strcmp(prefix, "SH") == 0) return eShelf;
    return eFridge;
}

static void typeToPrefix(eProductType type, char* outPrefix)
{
    switch (type)
    {
    case eFruitVegtable: strcpy(outPrefix, "FV"); break;
    case eFridge:        strcpy(outPrefix, "FR"); break;
    case eFrozen:        strcpy(outPrefix, "FZ"); break;
    case eShelf:         strcpy(outPrefix, "SH"); break;
    default:             strcpy(outPrefix, "FV"); break;
    }
}

/***************************************************************
 * readBits / writeBits (optional)
 ***************************************************************/
static unsigned int readBits(const unsigned char* buffer, int fromBit, int numBits)
{
    unsigned int val = 0;
    for (int i = 0; i < numBits; i++)
    {
        int byteIndex = (fromBit + i) / 8;
        int bitIndex = 7 - ((fromBit + i) % 8);
        unsigned int bit = (buffer[byteIndex] >> bitIndex) & 1;
        val = (val << 1) | bit;
    }
    return val;
}

static void writeBits(unsigned char* buffer, int fromBit, int numBits, unsigned int value)
{
    for (int i = 0; i < numBits; i++)
    {
        int byteIndex = (fromBit + i) / 8;
        int bitIndex = 7 - ((fromBit + i) % 8);
        unsigned int bit = (value >> (numBits - 1 - i)) & 1;
        if (bit)
            buffer[byteIndex] |= (1 << bitIndex);
        else
            buffer[byteIndex] &= ~(1 << bitIndex);
    }
}

/***************************************************************
 * The following 3 are unchanged from HW3
 ***************************************************************/
int saveCustomersToTextFile(const Customer* customerArr,
    int customerCount,
    const char* customersFileName)
{
    FILE* fp = fopen(customersFileName, "w");
    if (!fp)
    {
        printf("Error opening customers file '%s' for writing\n", customersFileName);
        return 0;
    }

    fprintf(fp, "%d\n", customerCount);
    for (int i = 0; i < customerCount; i++)
        customerArr[i].vTable.saveToFile(&customerArr[i], fp);

    fclose(fp);
    return 1;
}

Customer* loadCustomersFromTextFile(const char* customersFileName, int* pCount)
{
    FILE* fp = fopen(customersFileName, "r");
    if (!fp)
    {
        printf("Error opening customers file '%s' for reading\n", customersFileName);
        return NULL;
    }

    Customer* customerArr = NULL;
    int customerCount;
    fscanf(fp, "%d\n", &customerCount);

    if (customerCount > 0)
    {
        customerArr = (Customer*)calloc(customerCount, sizeof(Customer));
        if (!customerArr)
        {
            fclose(fp);
            return NULL;
        }
        for (int i = 0; i < customerCount; i++)
        {
            if (!loadCustomerFromFile(&customerArr[i], fp))
            {
                freeCustomerCloseFile(customerArr, i, fp);
                return NULL;
            }
        }
    }

    fclose(fp);
    *pCount = customerCount;
    return customerArr;
}

void freeCustomerCloseFile(Customer* customerArr,
    int count,
    FILE* fp)
{
    for (int i = 0; i < count; i++)
    {
        free(customerArr[i].name);
        customerArr[i].name = NULL;
        if (customerArr[i].pDerivedObj)
        {
            free(customerArr[i].pDerivedObj);
            customerArr[i].pDerivedObj = NULL;
        }
    }
    free(customerArr);
    fclose(fp);
}
