"""
Python script to generate Java AST classes.
"""

import os
from typing import List

PACKAGE: str = "lox"

def define_type(writer, base_name: str, class_name: str, field_list: str):
    """
    Define a single AST subclass with constructor and fields in Java syntax.
    """
    # Write class definition
    writer.write(f"  static class {class_name} extends {base_name} {{\n")

    # Constructor
    writer.write(f"    {class_name}({field_list}) {{\n")

    # Store parameters in fields
    fields = [f.strip() for f in field_list.split(",")]
    for field in fields:
        field_parts = field.strip().split(" ")
        field_type, field_name = field_parts[0], field_parts[1]
        writer.write(f"      this.{field_name} = {field_name};\n")

    writer.write("    }\n")

    # Fields
    writer.write("\n")
    for field in fields:
        writer.write(f"    final {field};\n")

    visitor_method_name = f"visit{class_name}{base_name}"
    writer.write("\n")
    writer.write("  @Override\n")
    writer.write("  <R> R accept(Visitor<R> visitor) {\n")
    writer.write(f"      return visitor.{visitor_method_name}(this);\n")
    writer.write("  }\n")

    writer.write("}\n")


def define_visitor(writer, base_name: str, types: List[str]) -> None:
    writer.write(" interface Visitor <R> {\n")
    for t in types:
        type_name = t.split(":")[0].strip()
        method_name = f"visit{type_name}{base_name}"

        writer.write(f"    R {method_name}({type_name} {base_name.lower()});\n")
    writer.write("  }\n")


def define_ast(output_dir: str, base_name: str, types: List[str]) -> None:
    """
    Generate an AST base class with subclasses in Java syntax.
    """
    path = os.path.join(output_dir, f"{base_name}.java")
    with open(path, "w", encoding="utf-8") as writer:
        # Write module header
        writer.write(f"package {PACKAGE};\n\n")
        writer.write("import java.util.List;\n\n")

        # Base class definition
        writer.write(f"abstract class {base_name} {{\n")

        define_visitor(writer, base_name, types)

        writer.write("  abstract <R> R accept(Visitor<R> visitor);\n\n")

        # Generate each AST subclass
        for type_def in types:
            class_part, fields_part = type_def.split(":", 1)
            class_name = class_part.strip()
            fields = fields_part.strip()
            define_type(writer, base_name, class_name, fields)

        writer.write("}")


def main() -> None:
    """
    Main function to generate the Java AST file.
    """
    output_dir = "../lox"
    # Ensure directory exists
    os.makedirs(output_dir, exist_ok=True)

    # Define AST types (same as in the original Java code)
    expr_types: List[str] = [
		"Assign   : Token name, Expr value",
        "Binary   : Expr left, Token operator, Expr right",
        "Grouping : Expr expression",
        "Literal  : Object value",
        "Unary    : Token operator, Expr right",
        "Variable : Token name"
    ]
    stmt_types: List[str] = [
        "Expression : Expr expression",
        "Print      : Expr expression",
        "Var        : Token name, Expr initializer"
    ]

    define_ast(output_dir, "Expr", expr_types)
    define_ast(output_dir, "Stmt", stmt_types)

    print("Success.")


if __name__ == "__main__":
    main()
