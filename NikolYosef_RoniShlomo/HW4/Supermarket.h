#pragma once
#include "Customer.h"
#include "Product.h"
#include "ShoppingCart.h"

typedef enum { eNone, eName, eCount, ePrice, eNofSortOpt } eSortOption;

typedef struct
{
	char*		name;
	Customer*	customerArr;
	int			customerCount;
	Product**	productArr;
	int			productCount;
	eSortOption sortOpt;
}SuperMarket;


int initSuperMarket(SuperMarket* pMarket,
	const char* fileName,
	const char* customersFileName,
	int isCompressed);

void		printSuperMarket(const SuperMarket* pMarket);
int			addProduct(SuperMarket* pMarket);
int			addNewProduct(SuperMarket* pMarket);
int			addCustomer(SuperMarket* pMarket);
int			doShopping(SuperMarket* pMarket);
Customer*	doPrintCart(SuperMarket* pMarket);
int			manageShoppingCart(SuperMarket* pMarket);
Customer*	getCustomerShopPay(SuperMarket* pMarket);
void		fillCart(SuperMarket* pMarket, ShoppingCart* pCart);
void		clearCart(SuperMarket* pMarket, Customer* pCustomer);

void		printProductByType(SuperMarket* pMarket);
void		printAllProducts(const SuperMarket* pMarket);
void		printAllCustomers(const SuperMarket* pMarket);
int			isBarcodeUnique(const SuperMarket* pMarket, const char* barcode);
int 		isCustomerIdUnique(const SuperMarket* pMarket, const char* id);


int			getProductIndexByBarcode(SuperMarket* pMarket, const char* barcode);
Product*	getProductByBarcode(SuperMarket* pMarket, const char* barcode);
Product*	getProductFromUser(SuperMarket* pMarket, char* barcode);
void		getUniquBarcode(char* barcode, SuperMarket* pMarket);
Customer*	FindCustomerById(SuperMarket* pMarket, const char* id);

eSortOption showSortMenu();
void*		getCompareFunction(eSortOption sort);
void		sortProducts(SuperMarket* pMarket);
void		findProduct(const SuperMarket* pMarket);

Customer*	getCustomerWhoShop(SuperMarket* pMarket);
Product*	getProductAndCount(SuperMarket* pMarket, int* pCount);

void		freeMarket(SuperMarket* pMarket);
void		freeProducts(SuperMarket* pMarket);
void		freeCustomers(SuperMarket* pMarket);
void		handleCustomerStillShoppingAtExit(SuperMarket* pMarket);

