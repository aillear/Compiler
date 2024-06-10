#pragma once
#include<string>
#include<iostream>
#include<vector>


namespace semanticAnalyzer {
	// 符号基类
	class NoteBase {

	};

	// 终结符
	class Terminal : NoteBase
	{
	public:
		std::string lexeme;
		Terminal();
	private:
		  
	};

	// 非终结符
	class NonTerminal : NoteBase {
	public:

	private:
	};

	class SemanticAnalyzer
	{
	public:
		static SemanticAnalyzer& Instance();
		void SetStartWith(int num);
		int tempCount = 0;		// 临时变量计数
		int nextInStr = 0;		// 三元式计数
		std::vector<NoteBase*> NotesFlow;	// 符号栈
		//int labelCount = 0;		// 
		void newTemp();
		void gen(std::string str);
		void put(std::string lexeme, std::string type);
		std::vector<int> makeList(int i);
		std::vector<int> merge(std::vector<int> p1, std::vector<int> p2);
		void backpatch(std::vector<int> p, int i);
	private:
		static SemanticAnalyzer* instance;
		SemanticAnalyzer();
		~SemanticAnalyzer();
	};


}