#pragma once
#include<iostream>
#include<unordered_map>
#include<set>
#include<vector>
#include"Tag.h"
#include <iomanip> 

namespace lexer {
	class Token 
	{
	public:
		const int tag;
		Token(Tag _tag) : tag(_tag) { ; }
		~Token();
		virtual std::string ToString();
	};

	class Num : public Token
	{
	public:
		const int value;
		Num(int v) : Token(Tag::NUM), value(v) { ; }
		~Num();
		std::string ToString() override;
	};

	class Real : public Token
	{
	public:
		const double value;
		Real(double v) : Token(Tag::REAL), value(v) { ; }
		~Real();
		std::string ToString() override;
	};

	class Word : public Token
	{
	public:
		std::string lexeme;
		Word(std::string str, Tag _tag) : Token(_tag), lexeme(str) { ; }
		~Word();
		std::string ToString() override;
	};

	class Type : public Word
	{
	public:
		int width;
		Type(std::string str, Tag _tag, int w) : Word(str, _tag), width(w) { ; }
		~Type();

	};

	// 定义符号表项
	struct SymbolEntry {
		std::string type;
		std::string name;
	};

	class SymbolTable
	{
	public:
		SymbolTable();
		~SymbolTable();
		void AddNewSymbol(const std::string& type, const std::string& name);
		void OutputTable(const std::string& path);
	private:
		std::vector<SymbolEntry> table;
	};

	class Lexer
	{
	public:
		// Singleton
		static Lexer& Instance();
		int line = 1;	// line 1
		char peek = ' ';	//  space
		std::unordered_map<std::string, Word> words;
		void SetFilePointer(FILE* _fp);
		void Reverse(Word w);
		void Readch();
		bool Readch(char c);
		Token* Scan();
		FILE* fp;
	private:
		static Lexer* instance;
		Lexer();
		~Lexer();
	};

	// use this to get singleton
	Lexer& GetLexer();
}