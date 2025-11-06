#include <stdio.h>
#include <stdlib.h>
#include "ClubMember.h"
#include "Customer.h"


int initClubMember(Customer* pCust)
{
	getCustomerID(pCust);
	if (!initClubMemberNoId(pCust))
		return 0;

	return 1;
}

int	initClubMemberNoId(struct Customer_* pCust)
{
	if (!initCustomerNoId(pCust))
		return 0;

	pCust->pDerivedObj = (ClubMember*)malloc(sizeof(ClubMember));
	if (!pCust->pDerivedObj)
	{
		freeCustomer(pCust);
		return 0;
	}

	ClubMember* pClubMember = (ClubMember*)pCust->pDerivedObj;
	do {
		printf("Please enter the total months of membership\n");
		scanf("%d", &pClubMember->totalMonths);
		if (pClubMember->totalMonths <= 0)
			printf("Total months should be positive\n");
	} while (pClubMember->totalMonths <= 0);

	initClubMemberVTable(pCust);

	pClubMember->pBase = pCust;
	return 1;
}

void initClubMemberVTable(Customer* pCust)
{
	pCust->vTable.print = printClubMember;
	pCust->vTable.deleteObj = freeClubMember;
	pCust->vTable.saveToFile = saveClubMemberToFile;
	pCust->vTable.discount = memberDiscount;
}

int initMemeberFromFile(Customer* pCust,FILE* fp)
{
	initRegFromFile(pCust, fp);
	ClubMember* pClub = (ClubMember*)calloc(1, sizeof(ClubMember));
	if (!pClub)
		return 0;

	if (fscanf(fp, "%d", &pClub->totalMonths) != 1)
		return 0;
	
	pCust->pDerivedObj = pClub;
	pClub->pBase = pCust;
	initClubMemberVTable(pCust);
	return 1;
}

void printClubMember(const Customer* pCust)
{
	printCustomer(pCust);
	ClubMember* pClubMember = (ClubMember*)pCust->pDerivedObj;
	printf("Total months of membership: %d\n", pClubMember->totalMonths);
}

void freeClubMember(struct Customer_* pCust)
{
	if (pCust->pDerivedObj)
		free(pCust->pDerivedObj);
	freeCustomer(pCust);
}

void saveClubMemberToFile(const  Customer* pCust, FILE* fp)
{
	saveCustomerToFile(pCust, fp);
	ClubMember* pClubMember = (ClubMember*)pCust->pDerivedObj;
	if (pClubMember)
		fprintf(fp, "%d %d\n", 1, pClubMember->totalMonths); 
}


float	memberDiscount(const Customer* pCustomer)
{
	int totalMonths = ((ClubMember*)pCustomer->pDerivedObj)->totalMonths;
	int totalYears = totalMonths / 12;

	if (totalYears < YEAR_1)
		return (float)(YEAR_1_DISCOUNT * totalMonths);
	else if (totalYears >= YEAR_1 && totalYears < YEAR_2)
		return (float)(BASE_2_DISCOUNT + (YEAR_2_DISCOUNT * totalYears));
	else
		return MAX_DISCOUNT;
}