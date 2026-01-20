#include "compiler.h"
#include "parser.h"
#include "debug.h"

extern Parser parser;
extern Chunk *compilingChunk;

Compiler *current = NULL;

static void initCompiler(Compiler *compiler)
{
    compiler->localCount = 0;
    compiler->scopeDepth = 0;
    current = compiler;
}

static void endCompiler()
{
    emitReturn();
#ifdef DEBUG_PRINT_CODE
    if (!parser.hadError)
    {
        disassembleChunk(currentChunk(), "code");
    }
#endif
}

bool compile(VM *vm, const char *source, Chunk *chunk)
{
    initScanner(source);
    Compiler compiler;
    initCompiler(&compiler);
    initParser();
    compilingChunk = chunk;

    advance();
    while (!match(TOKEN_EOF))
    {
        declaration(vm);
    }
    endCompiler();
    return !parser.hadError;
}
