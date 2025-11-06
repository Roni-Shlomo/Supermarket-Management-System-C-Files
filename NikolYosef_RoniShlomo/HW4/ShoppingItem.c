#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include  "ShoppingItem.h"


void printItem(const void* var)
{
	const ShoppingItem* pItem = (const ShoppingItem*)var;
	printf("Item %s count %d price per item %.2f\n", pItem->barcode, pItem->count, pItem->price);
}

ShoppingItem* createItem(const char* barcode, float price, int count)
{
	ShoppingItem* pItem = (ShoppingItem*)malloc(1 * sizeof(ShoppingItem));
	if (!pItem)
		return NULL;

	strcpy(pItem->barcode, barcode);
	pItem->price = price;
	pItem->count = count;
	return pItem;
}

void freeItem(void* val)
{
	ShoppingItem* pItem = (ShoppingItem*)val;
	free(pItem);
}

int compareShoppingItemsByBarcode(const void* var1, const void* var2)
{
	const ShoppingItem* pItem1 = (const ShoppingItem*)var1;
	const ShoppingItem* pItem2 = (const ShoppingItem*)var2;

	return strcmp(pItem1->barcode, pItem2->barcode);
}
