#ifndef clox_scanner_h
#define clox_scanner_h

typedef enum
{
    // Single-character tokens.
    TOKEN_LEFT_PAREN,
    TOKEN_RIGHT_PAREN,
    TOKEN_LEFT_BRACE,
    TOKEN_RIGHT_BRACE,
    TOKEN_COMMA,
    TOKEN_DOT,
    TOKEN_MINUS,
    TOKEN_PLUS,
    TOKEN_SEMICOLON,
    TOKEN_SLASH,
    TOKEN_STAR,
    // One or two character tokens.
    TOKEN_BANG,
    TOKEN_BANG_EQUAL,
    TOKEN_EQUAL,
    TOKEN_EQUAL_EQUAL,
    TOKEN_GREATER,
    TOKEN_GREATER_EQUAL,
    TOKEN_LESS,
    TOKEN_LESS_EQUAL,
    TOKEN_DIAMOND,
    // Ternary operators.
    TOKEN_TERNARY_IF,
    TOKEN_LESS_LESS,
    TOKEN_LESS_EQUAL_LESS,
    TOKEN_LESS_LESS_EQUAL,
    TOKEN_LESS_EQUAL_LESS_EQUAL,
    // Literals.
    TOKEN_IDENTIFIER,
    TOKEN_STRING,
    TOKEN_NUMBER,
    // Keywords.
    TOKEN_AND,
    TOKEN_CLASS,
    TOKEN_ELSE,
    TOKEN_EXIT,
    TOKEN_FALSE,
    TOKEN_FOR,
    TOKEN_FUN,
    TOKEN_IF,
    TOKEN_NIL,
    TOKEN_OR,
    TOKEN_PRINT,
    TOKEN_RETURN,
    TOKEN_SUPER,
    TOKEN_THIS,
    TOKEN_TRUE,
    TOKEN_VAR,
    TOKEN_WHILE,
    TOKEN_XOR,
    TOKEN_GOTO,
    TOKEN_SYMBOL,

    TOKEN_ERROR,
    TOKEN_EOF
} TokenType;

typedef struct
{
    const char *start;   // Beginning of current lexeme
    const char *current; // Points to current _character_
    int line;
} Scanner;

typedef struct
{
    TokenType type;
    const char *start;
    int length;
    int line;
} Token;

void initScanner(const char *source);
Token scanToken();

#endif