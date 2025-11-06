#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ShoppingCart.h"
#include "General.h"

void initCart(ShoppingCart* pCart)
{
	if (pCart == NULL)
		return;
	L_init(&pCart->shoppingItems);
}

float getTotalPrice(const ShoppingCart* pCart)
{
	float price = 0;
	NODE* currentNode = pCart->shoppingItems.head.next;

	while (currentNode != NULL)
	{
		ShoppingItem* pItem = (ShoppingItem*)currentNode->key;
		price += (pItem->price * pItem->count);
		currentNode = currentNode->next;
	}

	return price;
}

int addItemToCart(ShoppingCart* pCart, const char* barcode, float price, int count)
{
	ShoppingItem* pItem = getItemByBarcode(pCart, barcode);
	if (!pItem) //new item
	{
		pItem = createItem(barcode, price, count);
		if (!pItem)
			return 0;

		return insertNewShoppingItemToList(&pCart->shoppingItems, pItem);
	}
	else //item already in cart
	{
		pItem->count += count;
		return 1;
	}
}

int insertNewShoppingItemToList(LIST* pShoppingList, ShoppingItem* pitem)
{
	NODE* pN = pShoppingList->head.next; //first Node
	NODE* pPrevNode = &pShoppingList->head;
	ShoppingItem* pTemp;
	int compRes;

	while (pN != NULL)
	{
		pTemp = (ShoppingItem*)pN->key;
		compRes = strcmp(pTemp->barcode, pitem->barcode);
		if (compRes == 0) //found a shopping item with this barcode in the cart
		{
			printf("Not new product\n");
			return 0;
		}

		if (compRes > 0) //found a place for new item, the next one bigger
		{	
			if (!L_insert(pPrevNode, pitem))
				return 0;
			return 1;
		}

		pPrevNode = pN;
		pN = pN->next;
	}

	//insert at end
	if (!L_insert(pPrevNode, pitem))
		return 0;
	return 1;
}

float printShoppingCart(const ShoppingCart* pCart)
{
	float price = getTotalPrice(pCart);

	L_print(&pCart->shoppingItems, printItem);

	/*printf("Total bill to pay: %.2f\n", price);*/

	return price;
}



ShoppingItem* getItemByBarcode(ShoppingCart* pCart, const char* barcode)
{
	ShoppingItem temp = {"", 0.0, 0};
	strncpy(temp.barcode, barcode, BARCODE_LENGTH);
	temp.barcode[BARCODE_LENGTH] = '\0'; // Ensure null-termination
    const NODE* pN = L_find(pCart->shoppingItems.head.next, &temp, compareShoppingItemsByBarcode);


	if(pN != NULL)
		return (ShoppingItem*)pN->key;

	return NULL;
}

void freeShoppingCart(ShoppingCart* pCart)
{
	L_free(&pCart->shoppingItems, freeItem);
}