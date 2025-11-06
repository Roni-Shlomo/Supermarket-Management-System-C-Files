#pragma once


#define YEAR_1 2 // max months 23
#define YEAR_1_DISCOUNT	0.1F   // max 2.3
#define YEAR_2	5			// max months 59
#define BASE_2_DISCOUNT	2.5F
#define YEAR_2_DISCOUNT 0.5F	// max 4.5
#define MAX_DISCOUNT	7.5F	// max 7.5


struct Customer_;

typedef struct ClubMember_
{
	void* pBase;
	int totalMonths;
}ClubMember;

void	initClubMemberVTable(struct Customer_* pCust);
int		initClubMemberNoId(struct Customer_* pCust);
int		initMemeberFromFile(struct Customer_* pCust, FILE* fp);
float	memberDiscount(const struct Customer_* pCustomer);
int		initClubMember(struct Customer_* pCust);
void	printClubMember(const struct Customer_* pCust);
void	freeClubMember(struct Customer_* pCust);
void	saveClubMemberToFile(const struct Customer_* pCust, FILE* fp);