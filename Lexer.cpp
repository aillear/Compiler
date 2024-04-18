#include "Lexer.h"
using namespace lexer;

// singleton here
Lexer* Lexer::instance = nullptr;
Lexer& Lexer::Instance() {
	if (instance == nullptr) {
		instance = new Lexer();
	}
	return *instance;
}
Lexer& GetLexer() {
	return Lexer::Instance();
}

Lexer::Lexer() { ; }
Lexer::~Lexer() { ; }
