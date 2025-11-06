#pragma once
#include "ShoppingCart.h"
#include "vtable.h"

#define CUSTOMER_ID_LENGTH 9
#define NAMES_SEP " "
#define NAME_PARTS_SEP "- "

typedef struct Customer_
{
	char			sId[CUSTOMER_ID_LENGTH+1];
	char*			name;
	ShoppingCart*	pCart;
	void*			pDerivedObj;
	CustomerVTable vTable;
}Customer;

void		initCustomerVTable(Customer* pCustomer);
int			initRegFromFile(Customer* pCust, FILE* fp);
int			initCustomer(Customer* pCustomer);
int			initCustomerNoId(Customer* pCustomer);
void		getCustomerID(Customer* pCustomer);
void		printCustomer(const Customer* pCustomer);
void		printCustomerCart(const Customer* pCustomer);
float		customerDiscount(const Customer* pCustomer);
void		pay(Customer* pCustomer);
void		cancelShopping(Customer* pCustomer);
int			loadCustomerFromFile(Customer* pCust, FILE* fp);
void		freeCustomer(Customer* pCust);

int			isCustomerIdValid(const char* id);
void		saveCustomerToFile(const Customer* pCust, FILE* fp);

int			isCustomerById(const Customer* pCust, const char* id);
void		getNamePart(char* part, const char* msg);
void		upperLowerCustomerName(char* name);
char*		combineFirstLast(char** parts);



