package lox;

import static lox.TokenType.AND;
import static lox.TokenType.BANG;
import static lox.TokenType.BANG_EQUAL;
import static lox.TokenType.CLASS;
import static lox.TokenType.COMMA;
import static lox.TokenType.DOT;
import static lox.TokenType.ELSE;
import static lox.TokenType.EOF;
import static lox.TokenType.EQUAL;
import static lox.TokenType.EQUAL_EQUAL;
import static lox.TokenType.EXIT;
import static lox.TokenType.FALSE;
import static lox.TokenType.FOR;
import static lox.TokenType.FUN;
import static lox.TokenType.GREATER;
import static lox.TokenType.GREATER_EQUAL;
import static lox.TokenType.IDENTIFIER;
import static lox.TokenType.IF;
import static lox.TokenType.INSTANCE_OF;
import static lox.TokenType.LEFT_BRACE;
import static lox.TokenType.LEFT_PAREN;
import static lox.TokenType.LESS;
import static lox.TokenType.LESS_EQUAL;
import static lox.TokenType.MINUS;
import static lox.TokenType.NIL;
import static lox.TokenType.NUMBER;
import static lox.TokenType.OR;
import static lox.TokenType.PLUS;
import static lox.TokenType.PRINT;
import static lox.TokenType.RETURN;
import static lox.TokenType.RIGHT_BRACE;
import static lox.TokenType.RIGHT_PAREN;
import static lox.TokenType.SEMICOLON;
import static lox.TokenType.SLASH;
import static lox.TokenType.STAR;
import static lox.TokenType.STRING;
import static lox.TokenType.SUPER;
import static lox.TokenType.THIS;
import static lox.TokenType.TRUE;
import static lox.TokenType.VAR;
import static lox.TokenType.WHILE;
import static lox.TokenType.XOR;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class Scanner {
	private final List<Token> tokens = new ArrayList<>();
	private final String source;
	private int start = 0;
	private int current = 0;
	private int line = 1;
	private static final Map<String, TokenType> keywords;
	static {
		keywords = new HashMap<>();
		keywords.put("and", AND);
		keywords.put("class", CLASS);
		keywords.put("else", ELSE);
		keywords.put("false", FALSE);
		keywords.put("for", FOR);
		keywords.put("fun", FUN);
		keywords.put("if", IF);
		keywords.put("nil", NIL);
		keywords.put("or", OR);
		keywords.put("print", PRINT);
		keywords.put("return", RETURN);
		keywords.put("super", SUPER);
		keywords.put("this", THIS);
		keywords.put("true", TRUE);
		keywords.put("var", VAR);
		keywords.put("while", WHILE);
		keywords.put("exit", EXIT);
		keywords.put("instanceof", INSTANCE_OF);
		keywords.put("xor", XOR);
	}

	public Scanner(String source) {
		this.source = source;
	}

	private boolean isAtEnd() {
		return current >= source.length();
	}

	List<Token> scanTokens() {
		while (!isAtEnd()) {
			start = current;
			scanToken();
		}

		tokens.add(new Token(EOF, ",", null, line));
		return tokens;
	}

	private char advance() {
		return source.charAt(current++);
	}

	private String advanceTwice() {
		return source.charAt(current++) + "" + source.charAt(current++);
	}

	private void addToken(TokenType type, Object literal) {
		String text = source.substring(start, current);
		tokens.add(new Token(type, text, literal, line));
	}

	private void addToken(TokenType type) {
		addToken(type, null);
	}

	private boolean match(char expected) {
		if (isAtEnd())
			return false;
		if (source.charAt(current) != expected)
			return false;
		current++; // advance
		return true;
	}

	// It’s sort of like advance(), but doesn’t consume the character
	private char peek() {
		if (isAtEnd())
			return '\0';
		return source.charAt(current);
	}

	private char peekNext() {
		if (current + 1 >= source.length())
			return '\0';
		return source.charAt(current + 1);
	}

	private void string() {
		while (peek() != '"' && !isAtEnd()) {
			if (peek() == '\n')
				line++;
			advance();
		}

		if (isAtEnd()) {
			Lox.error(line, "Unterminated string.");
			return;
		}

		// The closing ".
		advance();

		// Trim the surrounding quotes.
		String value = source.substring(start + 1, current - 1);
		addToken(STRING, value);
	}

	private boolean isDigit(char c) {
		return c >= '0' && c <= '9';
	}

	private void number() {
		while (isDigit(peek()))
			advance(); // go further if next character is a digit as well

		if (peek() == '.' && isDigit(peekNext())) {
			// consume the '.'
			advance();
			while (isDigit(peek()))
				advance();
		}

		Double value = Double.valueOf(source.substring(start, current));
		addToken(NUMBER, value);
	}

	private boolean isAlpha(char c) {
		return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
	}

	private boolean isAlphaNumeric(char c) {
		return isAlpha(c) || isDigit(c);
	}

	private void identifier() {
		while (isAlphaNumeric(peek()))
			advance(); // greedy scan

		String text = source.substring(start, current);
		TokenType type = keywords.get(text);
		if (type == null)
			type = IDENTIFIER;
		addToken(type);
	}

	private void lineComment() {
		while (peek() != '\n' && !isAtEnd())
			advance();
	}

	private void multilineComment() {
		while (peek() != '*' && peekNext() != '/') {
			if (peek() == '\n' && !isAtEnd()) {
				line++;
			}
			advance();
		}
		advanceTwice();
	}

	private void scanToken() {
		char c = advance();
		switch (c) {
		case '/' -> {
			if (match('/')) {
				lineComment();
			} else if (match('*')) {
				multilineComment();
			} else {
				addToken(SLASH);
			}
		}
		case '(' -> addToken(LEFT_PAREN);
		case ')' -> addToken(RIGHT_PAREN);
		case '{' -> addToken(LEFT_BRACE);
		case '}' -> addToken(RIGHT_BRACE);
		case ',' -> addToken(COMMA);
		case '.' -> addToken(DOT);
		case '-' -> addToken(MINUS);
		case '+' -> addToken(PLUS);
		case ';' -> addToken(SEMICOLON);
		case '*' -> addToken(STAR);
		case '!' -> addToken(match('=') ? BANG_EQUAL : BANG);
		case '=' -> addToken(match('=') ? EQUAL_EQUAL : EQUAL);
		case '<' -> addToken(match('=') ? LESS_EQUAL : LESS);
		case '>' -> addToken(match('=') ? GREATER_EQUAL : GREATER);
		case ' ' -> {
		}
		case '\r' -> {
		}
		case '\t' -> {
		}
		case '\n' -> line++;
		case '"' -> string();
		default -> {
			if (isDigit(c)) {
				number();
			} else if (isAlpha(c)) {
				identifier();
			} else {
				Lox.error(line, "Unexpected character.");
			}
		}
		}
	}
}
