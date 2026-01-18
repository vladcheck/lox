package src;

import java.util.HashMap;
import java.util.Map;

public class Environment {
	final Environment enclosing;
	private final Map<String, Object> values = new HashMap<>();

	// global scope
	Environment() {
		enclosing = null;
	}

	// local scope
	Environment(Environment enclosing) {
		this.enclosing = enclosing;
	}

	void define(String name, Object value) {
		values.put(name, value);
	}

	void assign(Token name, Object value) throws RuntimeError {
		if (values.containsKey(name.lexeme)) {
			values.put(name.lexeme, value);
			return;
		}

		// go up the environment tree
		if (enclosing != null) {
			enclosing.assign(name, value);
			return;
		}

		throw new RuntimeError(name, "Undefined variable '%s'.".formatted(name.lexeme));
	}

	Object get(Token name) throws RuntimeError {
		if (values.containsKey(name.lexeme)) {
			return values.get(name.lexeme);
		}

		// go up the environment tree
		if (enclosing != null)
			return enclosing.get(name);

		throw new RuntimeError(name, "Undefined variable '%s'.".formatted(name.lexeme));
	}
}
