#pragma once



#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>



typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;

typedef float f32;
typedef double f64;



enum
{
    FILW_PATH_BUF_MAX = 260,
};


typedef enum FILW_Change
{
    FILW_Change_Add = 0,
    FILW_Change_Delete,
    FILW_Change_Modified,

    FILW_NumChanges
} FILW_Change;

typedef void(*FILW_Callback)(const char* dir, const char* filename, FILW_Change change);



typedef struct FILW_Context FILW_Context;

FILW_Context* FILW_newContext(void);
void FILW_contextFree(FILW_Context* ctx);
void FILW_contextAddFile(FILW_Context* ctx, const char* path, FILW_Callback cb);
void FILW_contextUpdate(FILW_Context* ctx);














































































