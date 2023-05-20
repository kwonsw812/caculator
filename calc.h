#pragma once
#include "std_lib_facilities.h"

const char let = 'L';
const char quit = 'Q';
const char print = ';';
const char number = '8';
const char name = 'a';
const char unset = 'u';
const char change = 'ch';
const char op_sqrt = 'S';
const char op_sin = 's';
const char op_cos = 'c';
const char op_tan = 't';

struct Token {
	char kind;
	double value;
	string name;
	Token(char ch) :kind(ch), value(0) { }
	Token(char ch, double val) :kind(ch), value(val) { }
	Token(char ch, string name) :kind(ch), name(name) { }
};

class Token_stream {
	bool full;
	Token buffer;

public:
	Token_stream() :full(0), buffer(0) { }

	Token get();
	void putback(Token);

	void ignore(char);
};

class Calculator 
{
public:
	Calculator();

	void calculate();

private:
	struct Variable {
		string name;
		double value;
		Variable(string n, double v) :name(n), value(v) { }
	};

	vector<Variable> names;

	void variable_change();

	void undeclaration();

	bool is_declared(string);

	double get_value(string);

	void set_value(string, double);

	double expression();

	double primary();

	double exponential();

	double term();

	double declaration();

	double statement();

	void clean_up_mess();

	const string prompt = "> ";
	const string result = "= ";
};