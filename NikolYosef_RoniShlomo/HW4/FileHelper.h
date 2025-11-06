#pragma once

int		writeStringToFile(const char* str, FILE* fp, const char* msg);
int		writeCharsToFile(const char* arr, int size, FILE* fp, const char* msg);
int		writeIntToFile(int val, FILE* fp, const char* msg);
int		writeFloatToFile(float val, FILE* fp, const char* msg);

char*	readStringFromFile(FILE* fp, const char* msg);
char*	readDynStringFromTextFile(FILE* fp);
int		readFixSizeStrFromFile(char* arr, FILE* fp, const char* msg);
int		readCharsFromFile(char* arr, int size, FILE* fp, const char* msg);
int		readIntFromFile(int* val, FILE* fp, const char* msg);
int		readFloatFromFile(float* val, FILE* fp, const char* msg);
