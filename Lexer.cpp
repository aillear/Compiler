#include "Lexer.h"
#include<string>
#include<cctype>
using namespace lexer;



// singleton here
Lexer* Lexer::instance = nullptr;
Lexer& Lexer::Instance() {
	if (instance == nullptr) {
		instance = new Lexer();
	}
	return *instance;
}
Lexer& lexer::GetLexer() {
	return Lexer::Instance();
}

void Lexer::SetFilePointer(FILE* _fp) {
	fp = _fp;
}

void Lexer::Readch() {
	peek = fgetc(fp);
}

bool Lexer::Readch(char c) {
	Readch();
	if (peek != c) return false;
	peek = ' ';
	return true;
}

void Lexer::Reverse(Word w) {
	words.insert(std::make_pair(w.lexeme, w));
}


Token Lexer::Scan() {
	// 处理空行
	for (;; Readch()) {
		if (peek == ' ' || peek == '\t') continue;
		else if (peek == '\n') line++;
		else if (peek == EOF) return Word("EOF", Tag::END);	// 
		else break;
	}

	
	switch (peek)
	{
	case '&':
		if (Readch('&')) return Word("and", Tag::LOGICAL_AND); else return Word(std::string(1, peek), Tag::BITWISE_AND);
	case '|':
		if (Readch('|')) return Word("or", Tag::LOGICAL_OR); else return Word(std::string(1, peek), Tag::BITWISE_OR);
	case '=':
		if (Readch('=')) return Word("equals", Tag::EQUALS); else return Word(std::string(1, peek), Tag::ASSIGN);
	case '!':
		if (Readch('=')) return Word("not_equals", Tag::NOT_EQUALS); else return Word(std::string(1, peek), Tag::LOGICAL_NOT);
	case '<':
		if (Readch('=')) return Word("less_equal", Tag::LESS_EQUAL); else return Word(std::string(1, peek), Tag::LESS_THAN);
	case '>':
		if (Readch('=')) return Word("greater_equal", Tag::GREATER_EQUAL); else return Word(std::string(1, peek), Tag::GREATER_THAN);
	}

	// 数字
	if (std::isdigit(peek)) {
		// int
		int v = peek - '0';
		Readch();
		while (std::isdigit(peek))
		{
			v = 10 * v + peek - '0';
		}
		if (peek != '.') return Num(v);
		// float
		double dv = v;
		double d = 10;
		while (true)
		{
			Readch();
			if (!std::isdigit(peek)) break;
			dv += (peek - '0') * 1.0 / d;
			d *= 10;
		}
		return Real(dv);
	}

	// 字母下划线->关键字/标识符
	if (std::isalpha(peek) || peek == '_') {
		std::string str;
		str.push_back(peek);
		Readch();
		while (std::isdigit(peek) || std::isalpha(peek) || peek == '_')
		{
			str.push_back(peek);
			Readch();
		}
		auto wordsPair = words.find(str);
		// 非空代表为关键字或者之前已经遇到的标识符
		if (wordsPair != words.end()) {
			return wordsPair->second;
		}
		Word w = Word(str, Tag::IDENTIFIER);
		Reverse(w);
		return w;
	}
	// 未定义的标识符
	return Word(std::string(1, peek), Tag::UNDEFINED);

}

Lexer::Lexer() { 
	Reverse(Word("if", Tag::IF));
	Reverse(Word("else", Tag::ELSE));
	Reverse(Word("while", Tag::WHILE));
	Reverse(Word("do", Tag::DO));
	Reverse(Word("break", Tag::BREAK));
	Reverse(Word("true", Tag::TRUE));
	Reverse(Word("false", Tag::FALSE));
	Reverse(Type("int", Tag::BASIC, 4));
	Reverse(Type("float", Tag::BASIC, 8));
	Reverse(Type("bool", Tag::BASIC, 1));
	Reverse(Type("char", Tag::BASIC, 1));
}
Lexer::~Lexer() { ; }


std::string lexer::Token::ToString() {
	return std::string("");
}


std::string Num::ToString() {
	return std::to_string(value);
}

std::string Real::ToString() {
	return std::to_string(value);
}

std::string lexer::Word::ToString()
{
	return lexeme;
}
