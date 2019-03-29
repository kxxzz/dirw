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
    DIRW_PATH_BUF_MAX = 260,
};


typedef enum DIRW_Change
{
    DIRW_Change_Add = 0,
    DIRW_Change_Delete,
    DIRW_Change_Modified,

    DIRW_NumChanges
} DIRW_Change;

typedef void(*DIRW_Callback)(const char* dir, const char* filename, DIRW_Change change);



typedef struct DIRW_Context DIRW_Context;

DIRW_Context* DIRW_newContext(void);
void DIRW_contextFree(DIRW_Context* ctx);

void DIRW_addDir(DIRW_Context* ctx, const char* path, DIRW_Callback cb);
void DIRW_poll(DIRW_Context* ctx);














































































