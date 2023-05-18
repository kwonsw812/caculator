#pragma once
#include "std_lib_facilities.h"

const char let = 'L';
const char quit = 'Q';
const char print = ';';
const char number = '8';
const char name = 'a';

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

	void caculate();

	string get_prompt() const { return prompt; }
	string get_result() const { return result; }

private:
	double get_value(string);

	void set_value(string, double);

	bool is_declared(string);

	double expression();

	double primary();

	double term();

	double declaration();

	double statement();

	void clean_up_mess();

	const string prompt = "> ";
	const string result = "= ";

	void make_const();
};