BUILD_DIR := build

default: clox jlox

# Run the tests for the final versions of clox and jlox.
test: debug jlox $(TEST_SNAPSHOT)
	@- dart $(TEST_SNAPSHOT) clox
	@ dart $(TEST_SNAPSHOT) jlox

# Run the tests for the final version of clox.
test_clox: debug $(TEST_SNAPSHOT)
	@ dart $(TEST_SNAPSHOT) clox

# Run the tests for the final version of jlox.
test_jlox: jlox $(TEST_SNAPSHOT)
	@ dart $(TEST_SNAPSHOT) jlox

$(TEST_SNAPSHOT): $(TOOL_SOURCES)
	@ mkdir -p build
	@ echo "Compiling Dart snapshot..."
	@ dart --snapshot=$@ --snapshot-kind=app-jit tool/bin/test.dart clox >/dev/null

# Compile a debug build of clox.
debug:
	@ $(MAKE) -f util/c.make NAME=cloxd MODE=debug SOURCE_DIR=c

# Compile and run the AST generator.
generate_ast:
	@ $(MAKE) -f util/java.make DIR=java PACKAGE=tool
	@ java -cp build/java com.craftinginterpreters.tool.GenerateAst \
			java/com/craftinginterpreters/lox

# Compile the Java interpreter .java files to .class files.
jlox: generate_ast
	@ $(MAKE) -f util/java.make DIR=java PACKAGE=lox

run_generate_ast = @ java -cp build/gen/$(1) \
			com.craftinginterpreters.tool.GenerateAst \
			gen/$(1)/com/craftinginterpreters/lox

.PHONY: book c_chapters clean clox compile_snippets debug default diffs \
	get java_chapters jlox serve split_chapters test test_all test_c test_java
