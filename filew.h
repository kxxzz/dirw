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
    FilW_PATH_BUF_MAX = 260,
};


typedef enum FilW_Change
{
    FilW_Change_Add = 0,
    FilW_Change_Delete,
    FilW_Change_Modified,

    FilW_NumChanges
} FilW_Change;

typedef void(*FilW_Callback)(const char* dir, const char* filename, FilW_Change change);



typedef struct FilW_Context FilW_Context;

FilW_Context* FilW_newContext(void);
void FilW_contextFree(FilW_Context* ctx);
void FilW_contextAddFile(FilW_Context* ctx, const char* path, FilW_Callback cb);
void FilW_contextUpdate(FilW_Context* ctx);














































































