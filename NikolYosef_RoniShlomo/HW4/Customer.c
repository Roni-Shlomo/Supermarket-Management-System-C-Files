#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Customer.h"
#include "General.h"
#include "ClubMember.h"
#include "FileHelper.h"


int	initCustomer(Customer* pCustomer)
{
	getCustomerID(pCustomer);

	if (!initCustomerNoId(pCustomer))
		return 0;

	return 1;
}

int	initCustomerNoId(Customer* pCustomer)
{
	char firstName[MAX_STR_LEN] = { 0 };
	char lastName[MAX_STR_LEN] = { 0 };

	char* parts[2] = { firstName,lastName };
	char* msgParts[2] = { "Enter customer first name\n" ,"Enter customer last name\n" };
	for (int i = 0; i < 2; i++)
	{
		getNamePart(parts[i], msgParts[i]);
		upperLowerCustomerName(parts[i]);
	}

	char* combineName = combineFirstLast(parts);
	if (!combineName)
		return 0;

	pCustomer->name = combineName;

	pCustomer->pCart = NULL;
	pCustomer->pDerivedObj = NULL;
	initCustomerVTable(pCustomer);
	return 1;
}


void getCustomerID(Customer* pCustomer)
{
	char msg[MAX_STR_LEN];
	sprintf(msg, "ID should be %d digits\n"
		"For example: 123456789\n", CUSTOMER_ID_LENGTH);
	char tempId[MAX_STR_LEN];

	do {
		getsStrFixSize(tempId, MAX_STR_LEN, msg);
	} while (!isCustomerIdValid(tempId));

	strcpy(pCustomer->sId,tempId);
}

void initCustomerVTable(Customer* pCustomer)
{
	pCustomer->vTable.print = printCustomer;
	pCustomer->vTable.deleteObj = freeCustomer;
	pCustomer->vTable.saveToFile = saveCustomerToFile;
	pCustomer->vTable.discount = customerDiscount;
}

void getNamePart(char* part, const char* msg)
{
	int ok = 0;
	do {
		getsStrFixSize(part, MAX_STR_LEN, msg);
		if (checkEmptyString(part))
			printf("Name can not be empty\n");
		else if (!checkAlphaSpaceStr(part))
			printf("Name should contain only letters\n");
		else
			ok = 1;
	} while (!ok);
}

void upperLowerCustomerName(char* name)
{
	toLowerStr(name);
	while (isspace(*name))
		name++;

	*name = toupper(*name);
}

char* combineFirstLast(char** parts)
{
	char temp[MAX_STR_LEN * 2] = { 0 };
	char** pNameParts = NULL;
	int count = 0;
	size_t totalLength = 0;

	for (int i = 0; i < 2; i++)
	{
		pNameParts = splitCharsToWords(parts[i], &count, NAMES_SEP, &totalLength);
		if (!pNameParts)
			return NULL;

		for (int j = 0; j < count; j++)
		{
			strcat(temp, pNameParts[j]);
			strcat(temp, NAMES_SEP);
			free(pNameParts[j]); //do not need
		}
		free(pNameParts);
		pNameParts = NULL;

		if (i == 0)
			strcat(temp, NAME_PARTS_SEP);
	}
	temp[strlen(temp) - 1] = '\0'; //remove last space
	
	char* combineName = myStrDup(temp);
	return combineName;
}

void printCustomer(const Customer* pCustomer)
{
	printf("Name: %s\n", pCustomer->name);
	printf("ID: %s\n", pCustomer->sId);

	if (pCustomer->pCart == NULL)
		printf("Shopping cart is empty!\n");
	else
	{
		printf("Doing shopping now!!!\n");
	}
}

int isCustomerIdValid(const char* id)
{
	if (strlen(id) != CUSTOMER_ID_LENGTH)
		return 0;
	for (int i = 0; i < CUSTOMER_ID_LENGTH; i++)
	{
		if (!isdigit(id[i]))
			return 0;
	}
	return 1;
}


void pay(Customer* pCustomer)
{
	if (!pCustomer->pCart)
		return;
	printf("---------- Cart info and bill for %s ----------\n", pCustomer->name);

	printCustomerCart(pCustomer);

	printf("!!! --- Payment was recived!!!! --- \n");
	freeShoppingCart(pCustomer->pCart);
	free(pCustomer->pCart);
	pCustomer->pCart = NULL;
}

void	printCustomerCart(const Customer* pCustomer)
{
	float price = printShoppingCart(pCustomer->pCart);

#ifdef PRINT_DETAIL
	// If PRINT_DETAIL is defined, print discount & final sum
	float discount = pCustomer->vTable.discount(pCustomer);
	if (discount == 0)
		printf("Total price for %s is %.2f\n", pCustomer->name, price);
	else {
		price -= price * (discount / 100.0f);
		printf("Total price for %s is %.2f, after discount of %.2f%%\n",
			pCustomer->name, price, discount);
	}
#else
	// If PRINT_DETAIL is NOT defined, skip final sum
	printf("Cart for %s (detail disabled)\n", pCustomer->name);
#endif
}

float	customerDiscount(const Customer* pCustomer)
{
	return 0;
}

void cancelShopping(Customer* pCustomer)
{
	if (!pCustomer->pCart)
		return;
	printf("!!! --- Purchase was canceled!!!! --- \n");
	freeShoppingCart(pCustomer->pCart);
	free(pCustomer->pCart);
	pCustomer->pCart = NULL;
}


int	isCustomerById(const Customer* pCust, const char* id)
{
	if (strcmp(pCust->sId, id) == 0)
		return 1;
	return 0;
}


void freeCustomer(Customer* pCust)
{
	if (pCust->pCart)
		pay(pCust); //will free every thing
	free(pCust->name);
}

void saveCustomerToFile(const Customer* pCust, FILE* fp)
{
	fprintf(fp, "%s\n%s\n", pCust->name, pCust->sId);
	if (!pCust->pDerivedObj)
		fprintf(fp, "%d\n", 0); // Not a club member
}

int loadCustomerFromFile(Customer* pCust, FILE* fp)
{
	pCust->name = readDynStringFromTextFile(fp);
	if (!pCust->name)
		return 0;

	if (fscanf(fp, "%s", pCust->sId) != 1)
	{
		free(pCust->name);
		return 0;
	}
	
	int isClub;
	if(fscanf(fp, "%d", &isClub)!= 1)
	{
		free(pCust->name);
		return 0;
	}

	if (isClub)
		initMemeberFromFile(pCust, fp);
	else
		initRegFromFile(pCust, fp);
	return 1;
}

int initRegFromFile(Customer* pCust, FILE* fp)
{
	pCust->pDerivedObj = NULL;
	initCustomerVTable(pCust);
	return 1;
}