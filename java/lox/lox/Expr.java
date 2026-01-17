package lox;

import java.util.List;

abstract class Expr {
 interface Visitor <R> {
    R visitBinaryExpr(Binary expr);
    R visitGroupingExpr(Grouping expr);
    R visitLiteralExpr(Literal expr);
    R visitUnaryExpr(Unary expr);
    R visitPolishReverseNotationExpr(PolishReverseNotation expr);
  }
  abstract <R> R accept(Visitor<R> visitor);

  static class Binary extends Expr {
    Binary(Expr left, Token operator, Expr right) {
      this.left = left;
      this.operator = operator;
      this.right = right;
    }

    final Expr left;
    final Token operator;
    final Expr right;

  @Override
  <R> R accept(Visitor<R> visitor) {
      return visitor.visitBinaryExpr(this);
  }
}
  static class Grouping extends Expr {
	Grouping(Expr expression) {
	  this.expression = expression;
	}
	
	final Expr expression;

  @Override
  <R> R accept(Visitor<R> visitor) {
      return visitor.visitGroupingExpr(this);
  }
}
  static class Literal extends Expr {
    Literal(Object value) {
      this.value = value;
    }

    final Object value;

  @Override
  <R> R accept(Visitor<R> visitor) {
      return visitor.visitLiteralExpr(this);
  }
}
  static class Unary extends Expr {
    Unary(Token operator, Expr right) {
      this.operator = operator;
      this.right = right;
    }

    final Token operator;
    final Expr right;

  @Override
  <R> R accept(Visitor<R> visitor) {
      return visitor.visitUnaryExpr(this);
  }
}
  static class PolishReverseNotation extends Expr {
    PolishReverseNotation(Expr left, Expr right, Token operator) {
      this.left = left;
      this.right = right;
      this.operator = operator;
    }

    final Expr left;
    final Expr right;
    final Token operator;

  @Override
  <R> R accept(Visitor<R> visitor) {
      return visitor.visitPolishReverseNotationExpr(this);
  }
}
}