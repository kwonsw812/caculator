#include "calc.h"

void Token_stream::putback(Token t)
{
	if (full) error("buffer full");
	buffer = t;
	full = true;
}

Token_stream ts;

Token Token_stream::get()
{
	if (full) { full = false; return buffer; }
	char ch;
	cin >> ch;
	switch (ch) {
	case '(':
	case ')':
	case '+':
	case '-':
	case '*':
	case '/':
	case '%':
	case ';':
	case '^':
	case '=':
		return Token(ch);
	case '.':
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	{
		cin.unget();
		double val;
		cin >> val;
		return Token(number, val);
	}
	default:
		if (isalpha(ch)) {
			string s;
			s += ch;
			while (cin.get(ch) && (isalpha(ch) || isdigit(ch))) s += ch;
			cin.unget();
			if (s == "sqrt") return Token(op_sqrt);
			if (s == "sin") return Token(op_sin);
			if (s == "cos") return Token(op_cos);
			if (s == "tan") return Token(op_tan);
			if (s == "let") return Token(let);
			if (s == "quit") return Token(quit);
			if (s == "unset") return Token(unset);
			return Token(name, s);
		}
		error("Bad token");
	}
}

void Token_stream::ignore(char c)
{
	if (full && c == buffer.kind) {
		full = false;
		return;
	}
	full = false;

	char ch;
	while (cin >> ch)
		if (ch == c) return;
}

Calculator::Calculator()
{
	names.push_back(Variable("e", 2.71828));
	names.push_back(Variable("pi", 3.141592));
}

double Calculator::get_value(string s)
{
	for (int i = 0; i < names.size(); ++i)
		if (names[i].name == s) return names[i].value;
	error("get: undefined name ", s);
}

void Calculator::set_value(string s, double d)
{
	for (int i = 0; i <= names.size(); ++i)
		if (names[i].name == s) {
			names[i].value = d;
			return;
		}
	error("set: undefined name ", s);
}

bool Calculator::is_declared(string s)
{
	for (int i = 0; i < names.size(); ++i)
		if (names[i].name == s) return true;
	return false;
}

double Calculator::primary()
{
	Token t = ts.get();
	switch (t.kind) {
	case '(':
	{	double d = expression();
	t = ts.get();
	if (t.kind != ')') error("'(' expected");
	return d;
	}
	case op_sqrt:
	{
		double d = primary();
		if (d < 0) error("No negative for sqrt");
		else return sqrt(d);
	}
	case op_sin:
	{
		double d = primary();
		return sin(d);
	}
	case op_cos:
	{
		double d = primary();
		return cos(d);
	}
	case op_tan:
	{
		double d = primary();
		return tan(d);
	}

	case '-':
		return -primary();
	case '+':
		return primary();
	case number:
		return t.value;
	case name:
		return get_value(t.name);
	default:
		error("primary expected");
	}
}

double Calculator::exponential()
{
	double left = primary();

	vector<double> exp;

	while (true) {
		Token t = ts.get();
		switch (t.kind) {
		case '^':
			exp.push_back(primary());
			break;
		default:
			if (exp.size() >= 2) {
				for (int i = exp.size(); i > 1; i--) {
					exp[i - 2] = pow(exp[i - 2], exp[i - 1]);
				}
				left = pow(left, exp[0]);
				ts.putback(t);
				return left;
			}
			else if (exp.size() == 1) {
				left = pow(left, exp[0]);
				ts.putback(t);
				return left;
			}
			else {
				ts.putback(t);
				return left;
			}
		}
	}
}

double Calculator::term()
{
	double left = exponential();
	while (true) {
		Token t = ts.get();
		switch (t.kind) {
		case '*':
			left *= primary();
			break;
		case '/':
		{	double d = primary();
		if (d == 0) error("divide by zero");
		left /= d;
		break;
		}
		default:
			ts.putback(t);
			return left;
		}
	}
}

double Calculator::expression()
{
	double left = term();
	while (true) {
		Token t = ts.get();
		switch (t.kind) {
		case '+':
			left += term();
			break;
		case '-':
			left -= term();
			break;
		default:
			ts.putback(t);
			return left;
		}
	}
}

double Calculator::declaration()
{
	Token t = ts.get();
	if (t.kind != name) error("name expected in declaration");
	string name = t.name;
	if (is_declared(name)) error(name, " declared twice");
	Token t2 = ts.get();
	if (t2.kind != '=') error("= missing in declaration of ", name);
	double d = expression();
	names.push_back(Variable(name, d));
	return d;
}

void Calculator::undeclaration()
{
	Token t = ts.get();
	if (t.name != "e" && t.name != "pi" && is_declared(t.name)) {
		for (int i = 2; i < names.size(); i++) {
			if (t.name == names[i].name) names.erase(names.begin() + i);
		}
		error("Done");
	}

	else if (t.name == "e" || t.name == "pi") {
		error("Cannot unset value");
	}

	else {
		error("Not declared");
	}
}

double Calculator::statement()
{
	Token t = ts.get();
	switch (t.kind) {
	case let:
		return declaration();
	case unset:
		undeclaration();
	default:
		ts.putback(t);
		return expression();
	}
}

void Calculator::clean_up_mess()
{
	ts.ignore(print);
}

void Calculator::calculate()
{
	while (true) try {
		cout << prompt;
		Token t = ts.get();
		while (t.kind == print) t = ts.get();
		if (t.kind == quit) return;
		ts.putback(t);
		cout << result << statement() << endl;
	}
	catch (runtime_error& e) {
		cerr << e.what() << endl;
		clean_up_mess();
	}
}
