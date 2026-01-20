#ifndef clox_compiler_h
#define clox_compiler_h

#include "chunk.h"

typedef struct
{
    Token name;
    int depth;
} Local;

typedef struct
{
    Local locals[UINT8_COUNT];
    int localCount;
    int scopeDepth;
} Compiler;

bool compile(VM *vm, const char *source, Chunk *chunk);

#endif