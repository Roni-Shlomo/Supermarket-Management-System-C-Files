#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>        
#include "main.h"
#include "General.h"
#include "Supermarket.h"
#include "SuperFile.h"

int menu()
{
    int option;
    printf("\n");
    printf("Please choose one of the following options\n");
    for (int i = 0; i < eNofOptions; i++)
        printf("%d - %s\n", i, menuStrings[i]);
    printf("%d - Quit\n", EXIT);

    scanf("%d", &option);
    //clean buffer
    char tav;
    scanf("%c", &tav);

    return option;
}

/*************************************************************
 * Variadic printMessage() per HW4 instructions
 *************************************************************/
void printMessage(const char* first, ...)
{
    va_list args;
    va_start(args, first);

    const char* str = first;
    while (str != NULL)
    {
        printf("%s ", str);
        str = va_arg(args, const char*);
    }
    printf("\n");

    va_end(args);
}

/*************************************************************
 * main
 *   - Expects 2 command-line args: <0|1> <binary_filename>
 *************************************************************/
int main(int argc, char* argv[])
{
    // HW4: 2 arguments -> <isCompressed> <filename>
    if (argc != 3)
    {
        printf("Usage: %s <0|1> <filename>\n", argv[0]);
        return 1;
    }

    int isCompressed = atoi(argv[1]);
    const char* fileName = argv[2];

    srand((unsigned int)time(NULL));

    SuperMarket market;

    // Updated initSuperMarket with 4th param: 'isCompressed'
    if (!initSuperMarket(&market, fileName, CUSTOMER_FILE_NAME, isCompressed))
    {
        printf("Error init Super Market\n");
        return 0;
    }

    int stop = 0;
    while (!stop)
    {
        int option = menu();
        switch (option)
        {
        case eShowSuperMarket:
            printSuperMarket(&market);
            break;

        case eAddProduct:
            if (!addProduct(&market))
                printf("Error adding product\n");
            break;

        case eAddCustomer:
            if (!addCustomer(&market))
                printf("Error adding customer\n");
            break;

        case eCustomerDoShopping:
            if (!doShopping(&market))
                printf("Error in shopping\n");
            break;

        case ePrintCart:
            doPrintCart(&market);
            break;

        case eCustomerManageShoppingCart:
            if (!manageShoppingCart(&market))
                printf("Error in shopping cart management\n");
            break;

        case eSortProducts:
            sortProducts(&market);
            break;

        case eSearchProduct:
            findProduct(&market);
            break;

        case ePrintProductByType:
            printProductByType(&market);
            break;

        case EXIT:
            // HW4 requirement: use variadic printMessage
            printMessage("Thank", "You", "For", "Shopping", "With", "Us", NULL);
            stop = 1;
            break;

        default:
            printf("Wrong option\n");
            break;
        }
    }

    // Handle any open carts before closing
    handleCustomerStillShoppingAtExit(&market);

    // Save in compressed/uncompressed mode
    if (!saveSuperMarketToFile(&market, fileName, CUSTOMER_FILE_NAME, isCompressed))
        printf("Error saving supermarket to file\n");

    freeMarket(&market);
    return 1;
}
