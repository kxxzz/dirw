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
    FILEW_PATH_BUF_MAX = 260,
};


typedef enum FILEW_Change
{
    FILEW_Change_Add = 0,
    FILEW_Change_Delete,
    FILEW_Change_Modified,

    FILEW_NumChanges
} FILEW_Change;

typedef void(*FILEW_Callback)(const char* dir, const char* filename, FILEW_Change change);



typedef struct FILEW_Context FILEW_Context;

FILEW_Context* FILEW_newContext(void);
void FILEW_contextFree(FILEW_Context* ctx);

void FILEW_addFile(FILEW_Context* ctx, const char* path, FILEW_Callback cb);
void FILEW_poll(FILEW_Context* ctx);














































































