package lox;

import lox.Expr.Assign;
import lox.Expr.Logical;
import lox.Expr.Variable;

class AstPrinter implements Expr.Visitor<String> {
	String print(Expr expr) {
		return expr.accept(this);
	}

	// evaluates expressions and puts them in
	// parentheses recursively
	private String parenthesize(String name, Expr... exprs) {
		StringBuilder builder = new StringBuilder();

		builder.append("(").append(name);
		for (Expr expr : exprs) {
			builder.append(" ");
			builder.append(expr.accept(this));
		}
		builder.append(")");

		return builder.toString();
	
	}

	@Override
	public String visitBinaryExpr(Expr.Binary expr) {
		return parenthesize(expr.operator.lexeme, expr.left, expr.right);
	}

	@Override
	public String visitGroupingExpr(Expr.Grouping expr) {
		return parenthesize("group", expr.expression);
	}

	@Override
	public String visitLiteralExpr(Expr.Literal expr) {
		if (expr.value == null)
			return "nil";
		return expr.value.toString();
	}

	@Override
	public String visitUnaryExpr(Expr.Unary expr) {
		return parenthesize(expr.operator.lexeme, expr.right);
	}

	@Override
	public String visitVariableExpr(Variable expr) {
		return expr.toString();
	}

	@Override
	public String visitAssignExpr(Assign expr) {
		return parenthesize("=", expr);
	}

	@Override
	public String visitLogicalExpr(Logical expr) {
		return parenthesize(expr.operator.lexeme, expr.left, expr.right);
	}
}
