#ifndef clox_chunk_h
#define clox_chunk_h

#include "memory.h"
#include "value.h"

typedef enum
{
    OP_RETURN,        // `return` statement
    OP_CONSTANT,      // 16-bit constant
    OP_CONSTANT_LONG, // 32-bit constant
    OP_NEGATE,        // unary negation
    OP_PRINT,         // print a
    OP_NIL,           // nil
    OP_TRUE,          // true
    OP_FALSE,         // false
    OP_NOT,           // !a
    OP_OR,            // a || b
    OP_XOR,           // a xor b
    OP_AND,           // a && b
    OP_EQUAL,         // a == b
    OP_GREATER,       // a > b
    OP_LESS,          // a < b
    OP_DIAMOND,       // a <> b
    OP_ADD,           // a + b
    OP_SUBTRACT,      // a - b
    OP_MULTIPLY,      // a * b
    OP_DIVIDE,        // binary division
    OP_POP,           // pop value off of stack and forgets it
    OP_DEFINE_GLOBAL, // define global variable
    OP_GET_GLOBAL,    // get global variable's value
    OP_SET_GLOBAL,    // sets a new value to global variable
} OpCode;

// Chunk acts like a dynamic array
typedef struct
{
    int count; // bytes in use
    int capacity;
    uint8_t *code;
    int *lines;
    ValueArray constants;
} Chunk;

void initChunk(Chunk *chunk);
void freeChunk(Chunk *chunk);
void writeChunk(Chunk *chunk, uint8_t byte, int line);
int addConstant(Chunk *chunk, Value value);

#endif