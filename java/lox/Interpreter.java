package lox;

import java.util.List;

public class Interpreter implements Expr.Visitor<Object>, Stmt.Visitor<Void> {
	private Environment environment = new Environment();

	// Tries to interpret a single expression at runtime.
	// Throws RuntimeError if expression is invalid.
	void interpret(List<Stmt> statements) {
		try {
			for (Stmt statement : statements) {
				execute(statement);
			}
		} catch (RuntimeError error) {
			Lox.runtimeError(error);
		}
	}

	@Override
	public Object visitAssignExpr(Expr.Assign expr) {
		Object value = evaluate(expr.value);
		environment.assign(expr.name, value);
		return value;
	}

	@Override
	public Void visitVarStmt(Stmt.Var stmt) {
		Object value = null;
		if (stmt.initializer != null) {
			value = evaluate(stmt.initializer);
		}

		environment.define(stmt.name.lexeme, value);
		return null;
	}

	@Override
	public Object visitVariableExpr(Expr.Variable expr) {
		return environment.get(expr.name);
	}

	@Override
	public Object visitBinaryExpr(Expr.Binary expr) { // <-- ИЗМЕНЕНО
		// left to right order
		Object left = evaluate(expr.left);
		Object right = evaluate(expr.right);

		switch (expr.operator.type) {
			case MINUS:
				checkNumberOperands(expr.operator, left, right);
				return (double) left - (double) right;
			case PLUS:
				if (left instanceof Double && right instanceof Double) {
					return (double) left + (double) right;
				}
				if (left instanceof String || right instanceof String) {
					return stringify(left) + stringify(right);
				}
				throw new RuntimeError(
						expr.operator,
						"Operands must be two numbers, two strings, or number and a string.");
			case SLASH:
				checkNumberOperands(expr.operator, left, right);
				if ((double) right == 0)
					throw new RuntimeError(
							expr.operator,
							"Division by zero.");
				return (double) left / (double) right;
			case STAR:
				checkNumberOperands(expr.operator, left, right);
				return (double) left * (double) right;
			case LESS_EQUAL:
				checkNumberOperands(expr.operator, left, right);
				return (double) left <= (double) right;
			case GREATER_EQUAL:
				checkNumberOperands(expr.operator, left, right);
				return (double) left >= (double) right;
			case LESS:
				checkNumberOperands(expr.operator, left, right);
				return (double) left < (double) right;
			case GREATER:
				checkNumberOperands(expr.operator, left, right);
				return (double) left > (double) right;
			case EQUAL_EQUAL:
				return isEqual(left, right);
			case BANG_EQUAL:
				return !isEqual(left, right);
		}

		// Unreachable.
		return null;
	}

	@Override
	public Object visitGroupingExpr(Expr.Grouping expr) { // <-- ИЗМЕНЕНО
		return evaluate(expr.expression);
	}

	@Override
	public Object visitLiteralExpr(Expr.Literal expr) { // <-- ИЗМЕНЕНО
		return expr.value;
	}

	@Override
	public Object visitUnaryExpr(Expr.Unary expr) { // <-- ИЗМЕНЕНО
		Object right = evaluate(expr.right);

		switch (expr.operator.type) {
			case MINUS:
				checkNumberOperand(expr.operator, right);
				return -(double) right;
			case BANG:
				return !isTruthy(right);
		}

		// Unreachable.
		return null;
	}

	@Override
	public Void visitExpressionStmt(Stmt.Expression stmt) { // <-- ИЗМЕНЕНО
		evaluate(stmt.expression);
		return null;
	}

	@Override
	public Void visitPrintStmt(Stmt.Print stmt) { // <-- ИЗМЕНЕНО
		Object value = evaluate(stmt.expression);
		System.out.println(stringify(value));
		return null;
	}

	private Object evaluate(Expr expr) {
		return expr.accept(this);
	}

	private void execute(Stmt stmt) {
		stmt.accept(this);
	}

	private boolean isTruthy(Object object) {
		if (object == null)
			return false;
		if (object instanceof Boolean)
			return (boolean) object;
		return true;
	}

	private boolean isEqual(Object a, Object b) {
		if (a == null && b == null)
			return true;
		else if (a == null)
			return false;
		else
			return a.equals(b);
	}

	private void checkNumberOperand(Token operator, Object operand) {
		if (operand instanceof Double)
			return;
		throw new RuntimeError(operator, "Operand must be a number.");
	}

	private void checkNumberOperands(Token operator, Object left, Object right) {
		if (left instanceof Double && right instanceof Double)
			return;
		throw new RuntimeError(operator, "Both operands must be numbers.");
	}

	private String stringify(Object object) {
		if (object == null)
			return "nil";

		if (object instanceof Double) {
			String text = object.toString();

			if (text.endsWith(".0")) {
				// remove unnecessary floating point
				text = text.substring(0, text.length() - 2);
			}
			return text;
		}

		return object.toString();
	}
}