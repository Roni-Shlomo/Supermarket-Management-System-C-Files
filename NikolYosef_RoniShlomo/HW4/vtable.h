#pragma once

struct Customer_;

typedef struct
{
	void	(*print)(const struct Customer_* pCustomer);
	float	(*discount)(const struct Customer_* pCustomer);
	void	(*deleteObj)(struct Customer_* pCust);
	void	(*saveToFile)(const struct Customer_* pCust, FILE* fp);
}CustomerVTable;