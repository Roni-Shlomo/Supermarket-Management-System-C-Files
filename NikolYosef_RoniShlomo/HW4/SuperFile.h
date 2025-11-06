#pragma once
#include <stdio.h>
#include "Supermarket.h"
#include "Customer.h"

/*
   Save the SuperMarket data (products + name) to a binary file.
   Also saves customers to a text file (unchanged from HW3).
   'isCompressed' = 1 => compressed binary logic
                    0 => uncompressed (HW3 style)
*/
int saveSuperMarketToFile(const SuperMarket* pMarket,
    const char* fileName,
    const char* customersFileName,
    int isCompressed);

/*
   Load the SuperMarket data (products + name) from a binary file.
   Also loads customers from a text file (unchanged from HW3).
   'isCompressed' = 1 => compressed binary logic
                    0 => uncompressed (HW3 style)
*/
int loadSuperMarketFromFile(SuperMarket* pMarket,
    const char* fileName,
    const char* customersFileName,
    int isCompressed);

/*
   Save all customers to a text file (unchanged from HW3).
*/
int  saveCustomersToTextFile(const Customer* customerArr,
    int customerCount,
    const char* customersFileName);

/*
   Load all customers from a text file (unchanged from HW3).
   Returns dynamically allocated array of customers.
   Sets pCount to the number of customers found.
*/
Customer* loadCustomersFromTextFile(const char* customersFileName,
    int* pCount);

/*
   Helper to free partially loaded customers if file read fails.
*/
void freeCustomerCloseFile(Customer* customerArr,
    int customerIndex,
    FILE* fp);

