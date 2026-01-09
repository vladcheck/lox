# Generated AST classes
from abc import ABC
from typing import Union, Any

class Expr(ABC):
    pass

  class Binary(Expr):
    def __init__(self, left: 'Expr', operator: 'Token', right: 'Expr'):
      self.'Expr' = 'Expr'
      self.'Token' = 'Token'
      self.'Expr' = 'Expr'

    'Expr': left:
    'Token': operator:
    'Expr': right:

  class Grouping(Expr):
    def __init__(self, expression: 'Expr'):
      self.'Expr' = 'Expr'

    'Expr': expression:

  class Literal(Expr):
    def __init__(self, value: object):
      self.object = object

    object: value:

  class Unary(Expr):
    def __init__(self, operator: 'Token', right: 'Expr'):
      self.'Token' = 'Token'
      self.'Expr' = 'Expr'

    'Token': operator:
    'Expr': right:

