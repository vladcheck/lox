# Makefile
JAVAC = javac
JFLAGS = -d bin/
MAIN = lox.Lox

SOURCES = $(wildcard lox/*.java)

all:
	@mkdir -p bin
	$(JAVAC) $(JFLAGS) $(SOURCES)

clean:
	rm -rf bin/*

# Запуск REPL (интерактивный режим)
run: all
	java -classpath bin $(MAIN)

# Запуск с файлом скрипта
run-file: all
	java -classpath bin $(MAIN) $(FILE)

.PHONY: all clean run run-file