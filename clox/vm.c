#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "debug.h"
#include "vm.h"
#include "compiler.h"

static void resetStack(VM *vm)
{
    vm->stackTop = vm->stack;
}

static void runtimeError(VM *vm, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

    size_t instruction = vm->ip - vm->chunk->code - 1;
    int line = vm->chunk->lines[instruction];
    fprintf(stderr, "[line %d] in script\n", line);
    resetStack(vm);
}

void initVM(VM *vm)
{
    vm->chunk = NULL;
    vm->ip = NULL;
    resetStack(vm);
}

void freeVM(VM *vm)
{
}

static Value peek(VM *vm, int distance)
{
    return vm->stackTop[-1 - distance];
}

static bool isFalsey(Value value)
{
    return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

static int diamond(VM *vm, Value a, Value b)
{
    if (!IS_NUMBER(a) || !IS_NUMBER(b))
    {
        runtimeError(vm, "Operands must be numbers.");
        return 0; // unreachable
    }
    if (AS_NUMBER(a) < AS_NUMBER(b))
        return -1;
    else if (AS_NUMBER(a) == AS_NUMBER(b))
        return 0;
    else
        return 1;
}

InterpretResult run(VM *vm)
{
#define READ_BYTE() (*vm->ip++)
#define READ_CONSTANT() (vm->chunk->constants.values[READ_BYTE()])
#define BINARY_OP(vm, valueType, op)                            \
    do                                                          \
    {                                                           \
        if (!IS_NUMBER(peek(vm, 0)) || !IS_NUMBER(peek(vm, 1))) \
        {                                                       \
            runtimeError(vm, "Operands must be numbers.");      \
            return INTERPRET_RUNTIME_ERROR;                     \
        }                                                       \
        double b = AS_NUMBER(pop(vm));                          \
        double a = AS_NUMBER(pop(vm));                          \
        push(vm, valueType(a op b));                            \
    } while (false)
#ifdef DEBUG_TRACE_EXECUTION
    printf("\t");
    for (Value *slot = vm->stack; slot < vm->stackTop; slot++)
    {
        printf("[ ");
        printValue(*slot);
        printf(" ]");
    }
    printf(" ");
    disassembleInstruction(vm->chunk, (int)(vm->ip - vm->chunk->code));
#endif

    uint8_t instruction;
    switch (instruction = READ_BYTE())
    {
    case OP_DIAMOND:
    {
        Value b = pop(vm);
        Value a = pop(vm);
        push(vm, NUMBER_VAL(diamond(vm, a, b)));
        break;
    }
    case OP_EQUAL:
    {
        Value b = pop(vm);
        Value a = pop(vm);
        push(vm, BOOL_VAL(valuesEqual(a, b)));
        break;
    }
    case OP_GREATER:
    {
        BINARY_OP(vm, BOOL_VAL, >);
        break;
    }
    case OP_LESS:
    {
        BINARY_OP(vm, BOOL_VAL, <);
        break;
    }
    case OP_CONSTANT:
    {
        Value constant = READ_CONSTANT();
        push(vm, constant);
        break;
    }
    case OP_CONSTANT_LONG:
    {
        Value constant = READ_CONSTANT();
        push(vm, constant);
        break;
    }
    case OP_NIL:
    {
        push(vm, NIL_VAL);
        break;
    }
    case OP_TRUE:
    {
        push(vm, BOOL_VAL(true));
        break;
    }
    case OP_FALSE:
    {
        push(vm, BOOL_VAL(false));
        break;
    }
    case OP_NEGATE:
    {
        if (!IS_NUMBER(peek(vm, 0)))
        {
            runtimeError(vm, "Operand must be a number.");
            return INTERPRET_RUNTIME_ERROR;
        }
        push(vm, NUMBER_VAL(-AS_NUMBER(pop(vm))));
        break;
    }
    case OP_NOT:
        push(vm, BOOL_VAL(isFalsey(pop(vm))));
        break;
    case OP_ADD:
        BINARY_OP(vm, NUMBER_VAL, +);
        break;
    case OP_SUBTRACT:
        BINARY_OP(vm, NUMBER_VAL, -);
        break;
    case OP_MULTIPLY:
        BINARY_OP(vm, NUMBER_VAL, *);
        break;
    case OP_DIVIDE:
        BINARY_OP(vm, NUMBER_VAL, /);
        break;
    case OP_RETURN:
    {
        printValue(pop(vm));
        printf("\n");
        return INTERPRET_OK;
    }
    }

    return INTERPRET_OK; // unreachable

#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP
}

InterpretResult interpret(VM *vm, const char *source)
{
    Chunk chunk;
    initChunk(&chunk);

    if (!compile(source, &chunk))
    {
        freeChunk(&chunk);
        return INTERPRET_COMPILE_ERROR;
    }

    vm->chunk = &chunk;
    vm->ip = vm->chunk->code;

    InterpretResult result = run(vm);

    freeChunk(&chunk);
    return result;
}

void push(VM *vm, Value value)
{
    *vm->stackTop = value;
    vm->stackTop++;
}

Value pop(VM *vm)
{
    vm->stackTop--;
    return *vm->stackTop;
}