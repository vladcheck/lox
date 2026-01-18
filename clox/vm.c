#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "vm.h"

void initVM(VM *vm)
{
    vm->chunk = NULL;
    vm->ip = NULL;
}

void freeVM(VM *vm)
{
}

InterpretResult run(VM *vm)
{
#define READ_BYTE() (*vm->ip++)
#define READ_CONSTANT() (vm->chunk->constants.values[READ_BYTE()])

    for (;;)
    {
        uint8_t instruction;
        switch (instruction = READ_BYTE())
        {
        case OP_CONSTANT:
        {
            Value constant = READ_CONSTANT();
            printValue(constant);
            printf("\n");
            break;
        }
        case OP_CONSTANT_LONG:
        {
            Value constant = READ_CONSTANT();
            printValue(constant);
            printf("\n");
            break;
        }
        case OP_RETURN:
        {
            return INTERPRET_OK;
        }
        }
    }

#undef READ_BYTE
#undef READ_CONSTANT
}

InterpretResult interpret(VM *vm, Chunk *chunk)
{
    vm->chunk = chunk;
    vm->ip = vm->chunk->code;
    return run(vm);
}
