#pragma once
#include "ShoppingItem.h"
#include "GeneralList.h"

typedef struct
{
	LIST	shoppingItems;
}ShoppingCart;


void			initCart(ShoppingCart* pCart);
float			getTotalPrice(const ShoppingCart* pCart);
ShoppingItem*	getItemByBarcode(ShoppingCart* pCart, const char* barcode);
int				addItemToCart(ShoppingCart* pCart, const char* name, float price, int count);
float			printShoppingCart(const ShoppingCart* pCart);
int				insertNewShoppingItemToList(LIST* pShoppingList, ShoppingItem* pitem);

void			freeShoppingCart(ShoppingCart* pCart);