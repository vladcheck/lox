#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "scanner.h"

Scanner scanner;

void initScanner(const char *source)
{
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}

static bool isDigit(char c)
{
    return c >= '0' && c <= '9';
}

static bool isAlpha(char c)
{
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_';
}

static bool isAlphaNumeric(char c)
{
    return isDigit(c) || isAlpha(c);
}

bool isAtEnd()
{
    return *scanner.current == '\0';
}

static char advance()
{
    scanner.current++;          // consume
    return scanner.current[-1]; // return last
}

static char peek()
{
    return *scanner.current; // doesn't consume!!!
}

static char peekNext()
{
    if (isAtEnd())
        return '\0';
    return scanner.current[1]; // doesn't consume!!!
}

Token makeToken(TokenType tokenType)
{
    Token token;
    token.type = tokenType;
    token.start = scanner.start;
    token.length = (int)(scanner.current - scanner.start);
    token.line = scanner.line;
    return token;
}

Token errorToken(char *const message)
{
    Token token;
    token.type = TOKEN_ERROR;
    token.start = message;
    token.length = (int)strlen(message);
    token.line = scanner.line;
    return token;
}

static bool match(char expected)
{
    if (isAtEnd())
        return false;
    if (*scanner.current != expected)
        return false;
    scanner.current++; // consume
    return true;
}

static Token string()
{
    while (peek() != '`' && !isAtEnd())
    {
        if (peek() == '\n')
            scanner.line++;
        advance();
    }

    if (isAtEnd())
        return errorToken("Unterminated string.");

    advance(); // The closing quote
    return makeToken(TOKEN_STRING);
}

static Token number()
{
    while (isDigit(peek()))
        advance();

    // Look for a fractional part.
    if (peek() == '.' && isDigit(peekNext()))
    {
        // Consume the ".".
        advance();

        while (isDigit(peek()))
            advance();
    }

    return makeToken(TOKEN_NUMBER);
}

TokenType checkKeyword(
    int start, int length, const char *rest, TokenType type)
{
    bool sameLength = scanner.current - scanner.start == start + length;
    if (sameLength && memcmp(scanner.start + start, rest, length) == 0)
    {
        return type;
    }

    return TOKEN_IDENTIFIER;
}

static TokenType identifierType()
{
#ifndef IS_THERE_SECOND_CHAR
#define IS_THERE_SECOND_CHAR() = scanner.current - scanner.start > 1;
    switch (scanner.start[0])
    {
    case 'a':
        return checkKeyword(1, 2, "nd", TOKEN_AND);
    case 'c':
        return checkKeyword(1, 4, "lass", TOKEN_CLASS);
    case 'e':
        if (scanner.current - scanner.start > 1)
        {
            switch (scanner.start[1])
            {
            case 'l':
                return checkKeyword(1, 2, "se", TOKEN_ELSE);
            case 'x':
                return checkKeyword(1, 2, "it", TOKEN_EXIT);
            }
        }
        break;
    case 'f':
        if (scanner.current - scanner.start > 1)
        {
            switch (scanner.start[1])
            {
            case 'a':
                return checkKeyword(2, 3, "lse", TOKEN_FALSE);
            case 'o':
                return checkKeyword(2, 1, "r", TOKEN_FOR);
            case 'u':
                return checkKeyword(2, 1, "n", TOKEN_FUN);
            }
        }
        break;
    case 'i':
        return checkKeyword(1, 1, "f", TOKEN_IF);
    case 'n':
        return checkKeyword(1, 2, "il", TOKEN_NIL);
    case 'o':
        return checkKeyword(1, 1, "r", TOKEN_OR);
    case 'p':
        return checkKeyword(1, 4, "rint", TOKEN_PRINT);
    case 'r':
        return checkKeyword(1, 5, "eturn", TOKEN_RETURN);
    case 's':
        return checkKeyword(1, 4, "uper", TOKEN_SUPER);
    case 't':
        if (scanner.current - scanner.start > 1)
        {
            switch (scanner.start[1])
            {
            case 'h':
                return checkKeyword(2, 2, "is", TOKEN_THIS);
            case 'r':
                return checkKeyword(2, 2, "ue", TOKEN_TRUE);
            }
        }
        break;
    case 'v':
        return checkKeyword(1, 2, "ar", TOKEN_VAR);
    case 'x':
        return checkKeyword(1, 2, "or", TOKEN_XOR);
    case 'w':
        return checkKeyword(1, 4, "hile", TOKEN_WHILE);
    }

    return TOKEN_IDENTIFIER;
#endif
}

static Token identifier()
{
    while (isAlpha(peek()) || isDigit(peek()))
        advance();
    return makeToken(identifierType());
}

static void skipWhitespace()
{
    for (;;)
    {
        char c = peek();
        switch (c)
        {
        case ' ':
        case '\r':
        case '\t':
            advance();
            break;
        case '\n':
            scanner.line++;
            advance();
            break;
        case '/':
            if (peekNext() == '/')
            {
                // A comment goes until the end of the line.
                while (peek() != '\n' && !isAtEnd())
                    advance();
            }
            else
            {
                return;
            }
            break;
        default:
            return;
        }
    }
}

Token scanToken()
{
    skipWhitespace();
    scanner.start = scanner.current;
    if (isAtEnd())
        return makeToken(TOKEN_EOF);

    char c = advance();
    if (isAlpha(c))
        return identifier();
    if (isDigit(c))
        return number();

    switch (c)
    {
    case '(':
        return makeToken(TOKEN_LEFT_PAREN);
    case ')':
        return makeToken(TOKEN_RIGHT_PAREN);
    case '{':
        return makeToken(TOKEN_LEFT_BRACE);
    case '}':
        return makeToken(TOKEN_RIGHT_BRACE);
    case ';':
        return makeToken(TOKEN_SEMICOLON);
    case ',':
        return makeToken(TOKEN_COMMA);
    case '.':
        return makeToken(TOKEN_DOT);
    case '-':
        return makeToken(TOKEN_MINUS);
    case '+':
        return makeToken(TOKEN_PLUS);
    case '/':
        return makeToken(TOKEN_SLASH);
    case '*':
        return makeToken(TOKEN_STAR);
    case '!':
        return makeToken(
            match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
    case '=':
        return makeToken(
            match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
    case '<':
        return makeToken(
            match('=') ? TOKEN_LESS_EQUAL : match('>') ? TOKEN_DIAMOND
                                                       : TOKEN_LESS);
    case '>':
        return makeToken(
            match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
    case '`':
        return string();
    }

    return errorToken("Unexpected character.");
}
