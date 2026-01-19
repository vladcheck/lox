#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "value.h"
#include "vm.h"

#define ALLOCATE_OBJ(vm, type, objectType) \
    (type *)allocateObject(vm, sizeof(type), objectType)

// Return an `Obj` of certain type from an array of bytes in heap
static Obj *allocateObject(VM *vm, size_t size, ObjType type)
{
    Obj *object = (Obj *)reallocate(NULL, 0, size);
    object->type = type;

    object->next = vm->objects;
    vm->objects = object;
    return object;
}

// Returns a `string object` from an array of bytes in heap
static ObjString *allocateString(VM *vm, char *chars, int length)
{
    ObjString *string = ALLOCATE_OBJ(vm, ObjString, OBJ_STRING);
    string->length = length;
    string->chars = chars;
    return string;
}

// Takes ownership of preallocated `ObjString` on heap.
// @return Pointer to `ObjString`'s first character
ObjString *takeString(VM *vm, char *chars, int length)
{
    return allocateString(vm, chars, length);
}

// Copies `ObjString` to heap
ObjString *copyString(VM *vm, const char *chars, int length)
{
    char *heapChars = ALLOCATE(char, length + 1); // get free space
    memcpy(heapChars, chars, length);             // copy characters from current array to new space
    heapChars[length] = '\0';                     // add null character at the end
    return allocateString(vm, heapChars, length); // get 'string object' representation of allocated memory
}

// Prints an `Obj` representation to stdout
void printObject(Value value)
{
    switch (OBJ_TYPE(value))
    {
    case OBJ_STRING:
        printf("%s", AS_CSTRING(value));
        break;
    }
}