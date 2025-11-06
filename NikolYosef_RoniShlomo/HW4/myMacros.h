#ifndef __MY_MACROS_H__
#define __MY_MACROS_H__

#include <stdio.h>
#include <stdlib.h>

/*
1) CHECK_RETURN_0
   Checking pointer. If NULL => return 0
*/
#define CHECK_RETURN_0(ptr)       \
    if ((ptr) == NULL)           \
        return 0

/*
2) CHECK_MSG_RETURN_0
   Checking pointer. If NULL => print message, then return 0
*/
#define CHECK_MSG_RETURN_0(ptr, msg) \
    if ((ptr) == NULL) {            \
        printf("%s\n", (msg));      \
        return 0;                   \
    }

/*
3) FREE_CLOSE_FILE_RETURN_0
   If ptr == NULL => free(ptr), close file, return 0
*/
#define FREE_CLOSE_FILE_RETURN_0(ptr, file) \
    if ((ptr) == NULL) {                   \
        fclose(file);                      \
        free(ptr);                         \
        return 0;                          \
    }

/*
4) CLOSE_RETURN_0
   Close file, return 0
*/
#define CLOSE_RETURN_0(file) \
    {                        \
        fclose(file);        \
        return 0;            \
    }

#endif // __MY_MACROS_H__
