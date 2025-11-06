#pragma once
#include "Product.h"

typedef struct
{
	char		barcode[BARCODE_LENGTH + 1];
	float		price;
	int			count;
}ShoppingItem;

void			printItem(const void* var);
ShoppingItem*	createItem(const char* barcode, float price, int count);
void			freeItem(void* pItem);
int				compareShoppingItemsByBarcode(const void* var1, const void* var2);