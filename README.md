# Supermarket Management System – C (File Handling & Compression)

This project was developed as part of the “Introduction to Programming Systems” course at Afeka College (2025A).  
It is the final stage of a multi-part supermarket management project, focusing on **file handling, data compression, and advanced C programming techniques**.

The program extends previous assignments (HW2 & HW3) by implementing binary file storage, bitwise compression, macros, and variadic functions.  
All data can be saved, loaded, and managed through both text and compressed binary files, while maintaining modular and memory-safe design.

## Key Features
- **File Handling:** Save and load data from text and binary files (`SuperMarket.bin`, `Customers.txt`).  
- **Bitwise Compression:** Compact product data using bit-level manipulation for efficient storage.  
- **Macros:** Custom preprocessor macros for safer and cleaner code (e.g., `CHECK_RETURN_0`, `CLOSE_RETURN_0`).  
- **Variadic Functions:** Implemented a flexible `printMessage()` function supporting variable parameters.  
- **Linked Lists & Dynamic Memory:** Continued use of dynamic data structures for managing products and customers.  
- **Conditional Compilation:** Added build flags to toggle detailed printing and debug info (`DETAIL_PRINT`).  

## Technologies
- C language  
- Bitwise operations and macros  
- File I/O (text and binary)  
- Linked lists and dynamic memory  
- Variadic functions and modular design  
- Visual Studio / Ubuntu environments  

## Example Usage
```c
printMessage("Saving", "supermarket", "data", "to", "binary", "file", NULL);
