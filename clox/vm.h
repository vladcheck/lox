#ifndef clox_vm_h
#define clox_vm_h

#include "chunk.h"
#include "value.h"

#define STACK_MAX 256 // Self-proclaimed

typedef struct
{
    Chunk *chunk;
    uint8_t *ip;            // Instruction Pointer
    Value stack[STACK_MAX]; // Keeps all constants during current chunk execution
    Value *stackTop;        // Points to where the next value to be pushed will go
} VM;

typedef enum
{
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
} InterpretResult;

void initVM(VM *vm);
void freeVM(VM *vm);
InterpretResult interpret(VM *vm, Chunk *chunk);
void push(VM *vm, Value value);
Value pop(VM *vm);

#endif