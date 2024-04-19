#include "Lexer.h"
#include<string>
using namespace lexer;



// singleton here
Lexer* Lexer::instance = nullptr;
Lexer& Lexer::Instance() {
	if (instance == nullptr) {
		instance = new Lexer();
	}
	return *instance;
}
void Lexer::Reverse(Word w) {
	words.insert(std::make_pair(w.lexeme, w));
}
Lexer& lexer::GetLexer() {
	return Lexer::Instance();
}

Token Lexer::Scan() {

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


std::string lexer::Token::ToString()
{
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
