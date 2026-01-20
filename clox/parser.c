#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "parser.h"
#include "scanner.h"
#include "compiler.h"

#ifdef DEBUG_PRINT_CODE
#include "debug.h"
#endif

Parser parser;
extern Compiler *compiler;
Chunk *compilingChunk;

static void expression(VM *vm);
static void statement(VM *vm);
void declaration(VM *vm);
static ParseRule *getRule(TokenType type);
static void parsePrecedence(VM *vm, Precedence precedence);

Chunk *currentChunk()
{
    return compilingChunk;
}

void initParser()
{
    parser.hadError = false;
    parser.panicMode = false;
}

static void errorAt(Token *token, const char *message)
{
    if (parser.panicMode)
        return; // Supress any errors if one is already found
    parser.panicMode = true;
    fprintf(stderr, "[line %d] Error", token->line);

    if (token->type == TOKEN_EOF)
    {
        fprintf(stderr, " at end");
    }
    else if (token->type == TOKEN_ERROR)
    {
        // Nothing.
    }
    else
    {
        fprintf(stderr, " at '%.*s'", token->length, token->start);
    }

    fprintf(stderr, ": %s\n", message);
    parser.hadError = true;
}

static void errorAtCurrent(const char *message)
{
    errorAt(&parser.current, message);
}

// Declare error at previous token
static void error(const char *message)
{
    errorAt(&parser.previous, message);
}

void advance()
{
    parser.previous = parser.current;

    for (;;)
    {
        parser.current = scanToken();
        if (parser.current.type != TOKEN_ERROR)
            break;

        errorAtCurrent(parser.current.start);
    }
}

static void consume(TokenType type, const char *message)
{
    if (parser.current.type == type)
    {
        advance();
        return;
    }

    errorAtCurrent(message);
}

static bool check(TokenType type)
{
    return parser.current.type == type;
}

bool match(TokenType type)
{
    if (!check(type))
        return false;
    advance();
    return true;
}

static void emitByte(uint8_t byte)
{
    writeChunk(currentChunk(), byte, parser.previous.line);
}

static void emitBytes(uint8_t byte1, uint8_t byte2)
{
    emitByte(byte1);
    emitByte(byte2);
}

void emitReturn()
{
    emitByte(OP_RETURN);
}

static uint8_t makeConstant(Value value)
{
    int constant = addConstant(currentChunk(), value);
    if (constant > UINT8_MAX)
    {
        error("Too many constants in one chunk.");
        return 0;
    }

    return (uint8_t)constant;
}

static void emitConstant(Value value)
{
    emitBytes(OP_CONSTANT, makeConstant(value));
}

static void emitConstantLong(Value value)
{
    emitBytes(OP_CONSTANT_LONG, makeConstant(value));
}

static void binary(VM *vm, bool canAssign)
{
    TokenType operatorType = parser.previous.type;
    ParseRule *rule = getRule(operatorType);
    parsePrecedence(vm, (Precedence)(rule->precedence + 1));

    switch (operatorType)
    {
    case TOKEN_BANG_EQUAL:
    {
        emitBytes(OP_EQUAL, OP_NOT);
        break;
    }
    case TOKEN_EQUAL_EQUAL:
    {
        emitByte(OP_EQUAL);
        break;
    }
    case TOKEN_GREATER:
    {
        emitByte(OP_GREATER);
        break;
    }
    case TOKEN_GREATER_EQUAL:
    {
        emitBytes(OP_LESS, OP_NOT);
        break;
    }
    case TOKEN_LESS:
    {
        emitByte(OP_LESS);
        break;
    }
    case TOKEN_LESS_EQUAL:
    {
        emitBytes(OP_GREATER, OP_NOT);
        break;
    }
    case TOKEN_DIAMOND:
    {
        emitByte(OP_DIAMOND);
        return;
    }
    case TOKEN_PLUS:
    {
        emitByte(OP_ADD);
        break;
    }
    case TOKEN_MINUS:
    {
        emitByte(OP_SUBTRACT);
        break;
    }
    case TOKEN_STAR:
    {
        emitByte(OP_MULTIPLY);
        break;
    }
    case TOKEN_SLASH:
    {
        emitByte(OP_DIVIDE);
        break;
    }
    default:
        return; // Unreachable.
    }
}

static void literal(VM *vm, bool canAssign)
{
    switch (parser.previous.type)
    {
    case TOKEN_FALSE:
        emitByte(OP_FALSE);
        break;
    case TOKEN_NIL:
        emitByte(OP_NIL);
        break;
    case TOKEN_TRUE:
        emitByte(OP_TRUE);
        break;
    default:
        return; // Unreachable.
    }
}

static void expression(VM *vm)
{
    parsePrecedence(vm, PREC_ASSIGNMENT);
}

static uint8_t identifierConstant(VM *vm, Token *name)
{
    return makeConstant(OBJ_VAL(copyString(vm, name->start, name->length)));
}

static uint8_t parseVariable(VM *vm, const char *errorMessage)
{
    consume(TOKEN_IDENTIFIER, errorMessage);
    return identifierConstant(vm, &parser.previous);
}

static void defineVariable(uint8_t global)
{
    emitBytes(OP_DEFINE_GLOBAL, global);
}

static void varDeclaration(VM *vm)
{
    uint8_t global = parseVariable(vm, "Expect variable name.");

    if (match(TOKEN_EQUAL))
        expression(vm);
    else
        emitByte(OP_NIL);

    consume(TOKEN_SEMICOLON, "Expect ';' after variable declaration.");
    defineVariable(global);
}

static void expressionStatement(VM *vm)
{
    expression(vm);
    consume(TOKEN_SEMICOLON, "Expect ';' after expression.");
    emitByte(OP_POP);
}

static void printStatement(VM *vm)
{
    expression(vm);
    consume(TOKEN_SEMICOLON, "Expect ';' after value.");
    emitByte(OP_PRINT);
}

// Skips entire line or expression until semicolon is met
static void synchronize()
{
    parser.panicMode = false;

    while (parser.current.type != TOKEN_EOF)
    {
        if (parser.previous.type == TOKEN_SEMICOLON)
            return;
        switch (parser.current.type)
        {
        case TOKEN_CLASS:
        case TOKEN_FUN:
        case TOKEN_VAR:
        case TOKEN_FOR:
        case TOKEN_IF:
        case TOKEN_WHILE:
        case TOKEN_PRINT:
        case TOKEN_RETURN:
            return;

        default:; // Do nothing.
        }

        advance();
    }
}

static void statement(VM *vm)
{
    if (match(TOKEN_PRINT))
        printStatement(vm);
    else
        expressionStatement(vm);
}

void declaration(VM *vm)
{
    if (match(TOKEN_VAR))
        varDeclaration(vm);
    else
        statement(vm);

    if (parser.panicMode)
        synchronize();
}

static void grouping(VM *vm, bool canAssign)
{
    expression(vm);
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

static void number(VM *vm, bool canAssign)
{
    double value = strtod(parser.previous.start, NULL);
    emitConstant(NUMBER_VAL(value));
}

static void string(VM *vm, bool canAssign)
{
    const char *start = parser.previous.start + 1; // to trim leading quote
    size_t end = (parser.previous.length - 1) - 1; // to trim trailing quote
    emitConstant(OBJ_VAL(copyString(vm, start, end)));
}

static void namedVariable(VM *vm, Token name, bool canAssign)
{
    uint8_t arg = identifierConstant(vm, &name);
    if (canAssign && match(TOKEN_EQUAL))
    {
        expression(vm);
        emitBytes(OP_SET_GLOBAL, arg);
    }
    else
        emitBytes(OP_GET_GLOBAL, arg);
}

static void variable(VM *vm, bool canAssign)
{
    namedVariable(vm, parser.previous, canAssign);
}

static void unary(VM *vm, bool canAssign)
{
    TokenType operatorType = parser.previous.type;

    // Compile the operand.
    parsePrecedence(vm, PREC_UNARY);

    // Emit the operator instruction.
    switch (operatorType)
    {
    case TOKEN_BANG:
        emitByte(OP_NOT);
        break;
    case TOKEN_MINUS:
        emitByte(OP_NEGATE);
        break;
    default:
        return; // Unreachable.
    }
}

ParseRule rules[] = {
    [TOKEN_LEFT_PAREN] = {grouping, NULL, PREC_NONE},
    [TOKEN_RIGHT_PAREN] = {NULL, NULL, PREC_NONE},
    [TOKEN_LEFT_BRACE] = {NULL, NULL, PREC_NONE},
    [TOKEN_RIGHT_BRACE] = {NULL, NULL, PREC_NONE},
    [TOKEN_COMMA] = {NULL, NULL, PREC_NONE},
    [TOKEN_DOT] = {NULL, NULL, PREC_NONE},
    [TOKEN_MINUS] = {unary, binary, PREC_TERM},
    [TOKEN_PLUS] = {NULL, binary, PREC_TERM},
    [TOKEN_SEMICOLON] = {NULL, NULL, PREC_NONE},
    [TOKEN_SLASH] = {NULL, binary, PREC_FACTOR},
    [TOKEN_STAR] = {NULL, binary, PREC_FACTOR},
    [TOKEN_BANG] = {unary, NULL, PREC_NONE},
    [TOKEN_BANG_EQUAL] = {NULL, binary, PREC_EQUALITY},
    [TOKEN_EQUAL] = {NULL, binary, PREC_NONE},
    [TOKEN_EQUAL_EQUAL] = {NULL, binary, PREC_EQUALITY},
    [TOKEN_GREATER] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_GREATER_EQUAL] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_LESS] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_LESS_EQUAL] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_DIAMOND] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_IDENTIFIER] = {variable, NULL, PREC_NONE},
    [TOKEN_STRING] = {string, NULL, PREC_NONE},
    [TOKEN_NUMBER] = {number, NULL, PREC_NONE},
    [TOKEN_AND] = {NULL, NULL, PREC_NONE},
    [TOKEN_CLASS] = {NULL, NULL, PREC_NONE},
    [TOKEN_ELSE] = {NULL, NULL, PREC_NONE},
    [TOKEN_FALSE] = {literal, NULL, PREC_NONE},
    [TOKEN_FOR] = {NULL, NULL, PREC_NONE},
    [TOKEN_FUN] = {NULL, NULL, PREC_NONE},
    [TOKEN_IF] = {NULL, NULL, PREC_NONE},
    [TOKEN_NIL] = {literal, NULL, PREC_NONE},
    [TOKEN_OR] = {NULL, NULL, PREC_NONE},
    [TOKEN_PRINT] = {NULL, NULL, PREC_NONE},
    [TOKEN_RETURN] = {NULL, NULL, PREC_NONE},
    [TOKEN_SUPER] = {NULL, NULL, PREC_NONE},
    [TOKEN_THIS] = {NULL, NULL, PREC_NONE},
    [TOKEN_TRUE] = {literal, NULL, PREC_NONE},
    [TOKEN_VAR] = {NULL, NULL, PREC_NONE},
    [TOKEN_WHILE] = {NULL, NULL, PREC_NONE},
    [TOKEN_XOR] = {NULL, NULL, PREC_NONE},
    [TOKEN_ERROR] = {NULL, NULL, PREC_NONE},
    [TOKEN_EOF] = {NULL, NULL, PREC_NONE},
    [TOKEN_EXIT] = {NULL, NULL, PREC_NONE},
};

static ParseRule *getRule(TokenType type)
{
    return &rules[type];
}

static void parsePrecedence(VM *vm, Precedence precedence)
{
    advance();
    ParseFn prefixRule = getRule(parser.previous.type)->prefix;
    if (prefixRule == NULL)
    {
        error("Expect expression.");
        return;
    }

    // The only time we allow an assignment is when parsing an assignment expression or top-level
    // expression like in an expression statement
    bool canAssign = precedence <= PREC_ASSIGNMENT;
    prefixRule(vm, canAssign);

    while (precedence <= getRule(parser.current.type)->precedence)
    {
        advance();
        ParseFn infixRule = getRule(parser.previous.type)->infix;
        infixRule(vm, canAssign);
    }

    if (canAssign && match(TOKEN_EQUAL))
        error("Invalid assignment target.");
}
