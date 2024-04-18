#pragma once
#include<iostream>
#include<unordered_map>
#include"Tag.h"

namespace lexer {
	class Lexer
	{
	public:
		// Singleton
		static Lexer& Instance() { ; }
		int line = 1;	// line 1
		char peek = 0;	// 0 means nothing
		std::unordered_map<std::string, int> words;
	private:
		static Lexer* instance;
		Lexer();
		~Lexer();
	};

	// use this to get singleton
	Lexer& GetLexer() { ; }

	enum Tag {};

	class Token
	{
	public:
		const int tag;
		Token(Tag _tag) : tag(_tag) { ; }
		~Token();
	};

	class Num : Token
	{
	public:
		const int value;
		Num(int v) : Token(Tag::NUM), value(v) { ; }
		~Num();
	};

	class Real
	{
	public:
		Real();
		~Real();

	private:

	};

	Real::Real()
	{
	}

	Real::~Real()
	{
	}
}