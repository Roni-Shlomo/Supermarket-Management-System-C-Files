#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Supermarket.h"
#include "Product.h"
#include "Customer.h"
#include "ClubMember.h"
#include "General.h"
#include "ShoppingCart.h"
#include "FileHelper.h"
#include "SuperFile.h"

static const char* sortOptStr[eNofSortOpt] = { "None", "Name", "Count", "Price" };


int initSuperMarket(SuperMarket* pMarket,
	const char* fileName,
	const char* customersFileName,
	int isCompressed)
{
	pMarket->customerCount = 0;
	pMarket->customerArr = NULL;
	pMarket->productCount = 0;
	pMarket->productArr = NULL;
	pMarket->sortOpt = eNone;

	// If the file doesn’t exist or fails, we fallback to manual init:
	if (loadSuperMarketFromFile(pMarket, fileName, customersFileName, isCompressed) == 1)
	{
		printf("Supermarket successfully loaded from files\n");
		return 1;
	}

	// If we get here => load failed => do a new init
	pMarket->name = getStrExactLength("Enter market name");
	if (!pMarket->name)
		return 0;

	return 1;
}

void printSuperMarket(const SuperMarket* pMarket)
{
	printf("Super Market Name: %s\t", pMarket->name);
	printf("\n");
	printAllProducts(pMarket);
	printf("\n");
	printAllCustomers(pMarket);
}

int addProduct(SuperMarket* pMarket)
{
	char answer;
	char barcode[BARCODE_LENGTH + 1];
	Product* pProd = NULL;

	printf("\n");
	printf("Adding new product? y/Y: ");
	scanf("%c", &answer);
	getchar();

	if (toupper(answer) == 'Y')
	{
		if (!addNewProduct(pMarket))
		{
			free(pProd);
			return 0;
		}
		return 1;
	}
	else if (pMarket->productCount > 0)
	{
		printf("Do you want to increase the amount of an existing product? y/Y: ");
		scanf("%c", &answer);
		getchar();
		if (toupper(answer) == 'Y')
		{
			printAllProducts(pMarket);
			pProd = getProductFromUser(pMarket, barcode);
			if (pProd != NULL) //This barcode exist in stock
				updateProductCount(pProd);
		}
	}
	else
		return 0;
	
	return 1;
}

int addNewProduct(SuperMarket* pMarket)
{
	Product** pTempArr = (Product**)realloc(pMarket->productArr, (pMarket->productCount + 1) * sizeof(Product*));
	if (!pTempArr)
	{
		freeProducts(pMarket);
		return 0;
	}
	pMarket->productArr = pTempArr;
	
	Product* pProd = (Product*)calloc(1, sizeof(Product));
	if (!pProd)
	{
		free(pMarket->productArr);
		return 0;
	}

	initProductNoBarcode(pProd);

	do
	{
		generateBarcode(pProd);
	} while (!isBarcodeUnique(pMarket, pProd->barcode)); //generate new barcode until it is unique
	pMarket->sortOpt = eNone;
	pMarket->productArr[pMarket->productCount] = pProd;
	pMarket->productCount++;

	return 1;
}

int isBarcodeUnique(const SuperMarket* pMarket, const char* barcode)
{
	for (int i = 0; i < pMarket->productCount; i++)
	{
		if (strcmp(pMarket->productArr[i]->barcode, barcode) == 0)
			return 0; // Barcode is not unique
	}
	return 1; // Barcode is unique
}

int isCustomerIdUnique(const SuperMarket* pMarket, const char* id)
{
	for (int i = 0; i < pMarket->customerCount; i++)
	{
		if (strcmp(pMarket->customerArr[i].sId, id) == 0)
		{
			printf("ID %s is not unique\n", id);
			return 0; // ID is not unique
		}
	}
	return 1; // ID is unique
}

int addCustomer(SuperMarket* pMarket)
{
	int isClubMember;
	Customer cust = { 0 };

	do {
		getCustomerID(&cust);
	} while (!isCustomerIdUnique(pMarket, cust.sId));
	
	printf("Is the customer a club member? 1 for yes, 0 for no: ");
	do {
		scanf("%d", &isClubMember);
		if (isClubMember != 0 && isClubMember != 1)
			printf("Invalid input, please enter 1 for yes, 0 for no: ");
	} while (isClubMember != 0 && isClubMember != 1);

	if (isClubMember)
		initClubMemberNoId(&cust);
	else
		initCustomerNoId(&cust);

	pMarket->customerArr = (Customer*)safeRealloc(pMarket->customerArr, (pMarket->customerCount + 1) * sizeof(Customer));
	if (!pMarket->customerArr)
	{
		cust.vTable.deleteObj(&cust);
		return 0;
	}

	pMarket->customerArr[pMarket->customerCount] = cust;
	pMarket->customerCount++;
	return 1;
}


int	doShopping(SuperMarket* pMarket)
{
	Customer* pCustomer = getCustomerShopPay(pMarket);
	if (!pCustomer)
		return 0;

	if (pCustomer->pCart == NULL)
	{
		pCustomer->pCart = (ShoppingCart*)malloc(sizeof(ShoppingCart));
		if (!pCustomer->pCart)
			return 0;
		initCart(pCustomer->pCart);
	}

	fillCart(pMarket, pCustomer->pCart);

	// Check if the cart is empty by checking if the head's next node is NULL
	if (pCustomer->pCart->shoppingItems.head.next == NULL) // did not buy anything
	{
		free(pCustomer->pCart);
		pCustomer->pCart = NULL;
	}

	printf("---------- Shopping ended ----------\n");
	return 1;
}

Customer*	doPrintCart(SuperMarket* pMarket)
{
	Customer* pCustomer = getCustomerShopPay(pMarket);
	if (!pCustomer)
		return NULL;
	if (pCustomer->pCart == NULL)
	{
		printf("Customer cart is empty\n");
		return NULL;
	}
	printCustomerCart(pCustomer);
	
	return pCustomer;
}

int	manageShoppingCart(SuperMarket* pMarket)
{
	Customer* pCustomer = doPrintCart(pMarket);
	char answer;

	if(!pCustomer)
		return 0;

	printf("Do you want to pay for the cart? y/Y, anything else to cancel shopping!\t");
	do {
		scanf("%c", &answer);
	} while (isspace(answer));

	getchar(); //clean the enter

	if (answer == 'y' || answer == 'Y')
		pay(pCustomer);
	else {
		clearCart(pMarket, pCustomer);
		cancelShopping(pCustomer);
	}
	return 1;

}

Customer* getCustomerShopPay(SuperMarket* pMarket)
{
	if (pMarket->customerCount == 0)
	{
		printf("No customer listed to market\n");
		return NULL;
	}

	if (pMarket->productCount == 0)
	{
		printf("No products in market - cannot shop\n");
		return NULL;
	}

	Customer* pCustomer = getCustomerWhoShop(pMarket);
	if (!pCustomer)
	{
		printf("this customer is not listed\n");
		return NULL;
	}

	return pCustomer;
}

void printAllProducts(const SuperMarket* pMarket)
{
	printf("There are %d products\n", pMarket->productCount);
	printf("%-20s %-10s\t", "Name", "Barcode");
	printf("%-20s %-10s %-20s %-15s\n", "Type", "Price", "Count In Stoke", "Expiry Date");
	printf("-------------------------------------------------------------------------------------------------\n");
	
	generalArrayFunction(pMarket->productArr, pMarket->productCount, sizeof(Product**), printProductPtr);
}

void printAllCustomers(const SuperMarket* pMarket)
{
	printf("There are %d listed customers\n", pMarket->customerCount);

	for (int i = 0; i < pMarket->customerCount; i++)
		pMarket->customerArr[i].vTable.print(&pMarket->customerArr[i]);
}


Customer* getCustomerWhoShop(SuperMarket* pMarket)
{
	printAllCustomers(pMarket);
	char searchTerm[MAX_STR_LEN];
	getsStrFixSize(searchTerm, sizeof(searchTerm), "Who is shopping? Enter customer id\n");

	Customer* pCustomer = FindCustomerById(pMarket, searchTerm);
	
	return pCustomer;
}


void fillCart(SuperMarket* pMarket, ShoppingCart* pCart)
{
	printAllProducts(pMarket);
	char op;
	while (1)
	{
		printf("Do you want to shop for a product? y/Y, anything else to exit!!\t");
		do {
			scanf("%c", &op);
		} while (isspace(op));

		getchar(); //clean the enter

		if (op != 'y' && op != 'Y')
			break;

		int count;

		Product* pProd = getProductAndCount(pMarket, &count);

		if(pProd)
		{
			if (!addItemToCart(pCart, pProd->barcode, pProd->price, count))
			{
				printf("Error adding item\n");
				return;
			}
			pProd->count -= count; //item bought!!!
		}
	}
}

void clearCart(SuperMarket* pMarket, Customer* pCustomer)
{
	if (pCustomer->pCart == NULL)
		return;

	NODE* currentNode = pCustomer->pCart->shoppingItems.head.next;

	while (currentNode != NULL)
	{
		ShoppingItem* pItem = (ShoppingItem*)currentNode->key;
		Product* pProd = getProductByBarcode(pMarket, pItem->barcode);
		if (pProd)
			pProd->count += pItem->count; // Return the items to the stock

		currentNode = currentNode->next;
	}
}

Product* getProductAndCount(SuperMarket* pMarket, int* pCount)
{
	char barcode[BARCODE_LENGTH + 1];
	Product* pProd = getProductFromUser(pMarket, barcode);

	if (pProd == NULL)
	{
		printf("No such product\n");
		return NULL;
	} 
	
	if (pProd->count == 0)
	{
		printf("This product is out of stock\n");
		return NULL;
	}
	
	int count;

	do {
		printf("How many items do you want? max %d\n", pProd->count);
		scanf("%d", &count);
	} while (count <= 0 || count > pProd->count);

	*pCount = count;
	return pProd;
}

void printProductByType(SuperMarket* pMarket)
{
	if (pMarket->productCount == 0)
	{
		printf("No products in market\n");
		return;
	}

	eProductType type = getProductType();
	int count = 0;

	for (int i = 0; i < pMarket->productCount; i++)
	{
		if (pMarket->productArr[i]->type == type)
		{
			count++;
			printProduct(pMarket->productArr[i]);
		}
	}

	if (count == 0)
		printf("There are no product of type %s in market %s\n", getProductTypeStr(type), pMarket->name);
}

Product* getProductFromUser(SuperMarket* pMarket, char* barcode)
{
	getBarcodeCode(barcode);
	Product* pProd = getProductByBarcode(pMarket, barcode);

	if (!pProd)
	{
		printf("No such product barcode\n");
		return NULL;
	}

	return pProd;
}

void freeMarket(SuperMarket* pMarket)
{
	free(pMarket->name);
	freeProducts(pMarket);
	freeCustomers(pMarket);
}

void freeProducts(SuperMarket* pMarket)
{
	for (int i = 0; i < pMarket->productCount; i++)
	{
		freeProduct(pMarket->productArr[i]);
		free(pMarket->productArr[i]);
	}
	free(pMarket->productArr);
}

void freeCustomers(SuperMarket* pMarket)
{
	for (int i = 0; i < pMarket->customerCount; i++)
		pMarket->customerArr[i].vTable.deleteObj(&pMarket->customerArr[i]);
	free(pMarket->customerArr);
}

void	getUniquBarcode(char* barcode, SuperMarket* pMarket)
{
	int cont = 1;

	while (cont)
	{
		getBarcodeCode(barcode);
		int index = getProductIndexByBarcode(pMarket, barcode);

		if (index == -1)
			cont = 0;
		else
			printf("This product already in market\n");
	}
}

int getProductIndexByBarcode(SuperMarket* pMarket, const char* barcode)
{
	for (int i = 0; i < pMarket->productCount; i++)
	{
		if (isProduct(pMarket->productArr[i], barcode))
			return i;
	}
	return -1;
}

Product* getProductByBarcode(SuperMarket* pMarket, const char* barcode)
{
	for (int i = 0; i < pMarket->productCount; i++)
	{
		if (isProduct(pMarket->productArr[i], barcode))
			return pMarket->productArr[i];
	}
	return NULL;

}


Customer* FindCustomerById(SuperMarket* pMarket, const char* id)
{
	for (int i = 0; i < pMarket->customerCount; i++)
	{
		if (isCustomerById(&pMarket->customerArr[i], id))
			return &pMarket->customerArr[i];
	}
	return  NULL;
}

eSortOption showSortMenu()
{
	int opt;
	printf("Base on what field do you want to sort?\n");
	do {
		for (int i = 1; i < eNofSortOpt; i++)
			printf("Enter %d for %s\n", i, sortOptStr[i]);
		scanf("%d", &opt);
	} while (opt < 0 || opt >eNofSortOpt);

	return (eSortOption)opt;
}

void* getCompareFunction(eSortOption sort)
{
	int(*compare)(const void* air1, const void* air2) = NULL;

	switch (sort)
	{
	case eName:
		compare = compareProductsByName;
		break;
	case eCount:
		compare = compareProductsByCount;
		break;
	case ePrice:
		compare = compareProductsByPrice;
		break;
	}
	return compare;

}

void sortProducts(SuperMarket* pMarket)
{
	pMarket->sortOpt = showSortMenu();
	int (*compare)(const void* arg1, const void* arg2) = NULL;

	compare = getCompareFunction(pMarket->sortOpt);

	if (compare)
		qsort(pMarket->productArr, pMarket->productCount, sizeof(Product*), compare);
	else
		printf("Error in sorting\n");

}

void findProduct(const SuperMarket* pMarket)
{
	int (*compare)(const void* arg1, const void* arg2) = NULL;
	Product prod = { 0 };
	Product* pProd = &prod;

	compare = getCompareFunction(pMarket->sortOpt);

	switch (pMarket->sortOpt)
	{
	case eName:
		strcpy(pProd->name, getStrExactLength("Enter product name"));
		break;
	case eCount:
		printf("Enter product count\n");
		scanf("%d", &pProd->count);
		break;
	case ePrice:
		printf("Enter product price\n");
		scanf("%f", &pProd->price);
		break;
	case eNone:
		printf("Need to sort products first!!!\n");
		break;
	}

	if (compare)
	{
		Product** pTemp = (Product**)bsearch(&pProd, pMarket->productArr, pMarket->productCount, sizeof(Product*), compare);
		if (!pTemp)
			printf("Product not found\n");
		else
		{
			printf("Product found\n");
			printProduct(*pTemp);
		}
	}
	else {
		printf("The search cannot be performed, array not sorted\n");
	}
}

void handleCustomerStillShoppingAtExit(SuperMarket* pMarket)
{
	Customer* pCust;
	for (int i = 0; i < pMarket->customerCount; i++)
	{
		pCust = &pMarket->customerArr[i];
		if (pCust->pCart)
		{
			printf("Market is closing must pay!!!\n");
			pay(pCust); //will free every thing and update shope info
		}
	}
}