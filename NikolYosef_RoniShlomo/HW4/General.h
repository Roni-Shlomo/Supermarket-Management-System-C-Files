#pragma once

#define MAX_STR_LEN 255
#define strdup myStrDup

char*	getStrExactLength(const char* msg);
char*	getsStrFixSize(char* buffer, int size, const char* msg);
char*	myGets(char* buffer, int size, FILE* source);
char*	getDynStr(char* str);
char**	splitCharsToWords(char* str, int* pCount, const char* del, size_t* pTotalLength);
char* 	myStrDup(const char* str);

float	getPositiveFloat(const char* msg);
int		getPositiveInt(const char* msg);
int		countCharInString(const char* str, char tav);
int		checkEmptyString(char* str);

void*	safeRealloc(void* ptr, size_t size);
int		checkAlphaSpaceStr(const char* str);
void	toLowerStr(char* str);

void	generalArrayFunction(void* arr, int size, int typeSize, void(*func)(void* element));

