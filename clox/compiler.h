#ifndef clox_compiler_h
#define clox_compiler_h

#include "object.h"
#include "scanner.h"
#include "chunk.h"

bool compile(VM *vm, const char *source, Chunk *chunk);

#endif
