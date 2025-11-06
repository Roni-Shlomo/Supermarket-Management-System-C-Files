#pragma once

#define MIN_YEAR 2024
#define MAX_YEAR 2030

#define MAX_DATE_STR_LEN 11 // dd/mm/yyyy + null terminator
#define DATE_FORMAT_LEN 8 // ddmmyyyy

typedef struct
{
	int		day;
	int		month;
	int		year;
}Date;

int		initDate(Date* pDate);
int		isValidDate(int day, int month, int year);
void	printDate(const Date* pDate);
char*	getDateStr(const Date* pDate);
int		saveDateToFile(const Date* pDate, FILE* fp);
int		loadDateFromFile(Date* pDate, FILE* fp);
