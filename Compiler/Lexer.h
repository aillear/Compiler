#pragma once
#include "Tag.h"
#include <iomanip>
#include <iostream>
#include <set>
#include <unordered_map>
#include <vector>

namespace lexer
{
	class Token // Token基类
	{
	public:
		const int tag;
		Token(Tag _tag) : tag(_tag) { ; }
		~Token();
		virtual std::string ToString();
	};

	class Num : public Token // 数字
	{
	public:
		const int value;
		Num(int v) : Token(Tag::NUM), value(v) { ; }
		~Num();
		std::string ToString() override;
	};

	class Real : public Token // 实数
	{
	public:
		const double value;
		Real(double v) : Token(Tag::REAL), value(v) { ; }
		~Real();
		std::string ToString() override;
	};

	class Word : public Token // 单词
	{
	public:
		std::string lexeme;
		Word(std::string str, Tag _tag) : Token(_tag), lexeme(str) { ; }
		~Word();
		std::string ToString() override;
	};
	
	class Type : public Word // 类型
	{
	public:
		int width;
		Type(std::string str, Tag _tag, int w) : Word(str, _tag), width(w) { ; }
		~Type();
	};

	// 定义符号表项
	struct SymbolEntry
	{
		std::string type;
		std::string name;
	};

	class OldSymbolTable
	{	
	public:
		OldSymbolTable();
		~OldSymbolTable();
		void AddNewSymbol(const std::string &type, const std::string &name);
		void OutputTable(const std::string &path);

	private:
		std::vector<SymbolEntry> table;
	};

	class Lexer // 词法分析器
	{
	public:
		// Singleton
		static Lexer &Instance();
		int line = 1;	 // line 1
		int row = 1;	 // row 1
		char peek = ' '; //  space
		Token* TopToken = nullptr;
		std::unordered_map<std::string, Word> words;
		void SetFilePointer(FILE *_fp);
		void Reverse(Word w);
		void Readch();
		bool Readch(char c);
		Token *Scan();
		Token* FixedScan(); // scan and save current token to top
		FILE *fp;

	private:
		static Lexer *instance;
		Lexer();
		~Lexer();
	};

	// use this to get singleton
	Lexer &GetLexer();
}