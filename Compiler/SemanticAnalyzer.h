#pragma once
#include<string>
#include<iostream>
#include<vector>
#include<unordered_set>
#include<map>
#include<unordered_map>
#include<stack>

namespace semanticAnalyzer {

	// 符号基类
	class NoteBase {
	public:
		std::string lexeme;
	};

	// 终结符
	class Terminal : NoteBase
	{
	public:
		Terminal();
	private:
		  
	};

	// 非终结符
	class NonTerminal : NoteBase {
	public:

	private:
	};

	struct SymbolType
	{
		// 此项为false,则该类型不为数组
		bool isArray;
		int width;		// 此类型的宽度
		std::string baseType;  // 数组基类型.如果isArray==false,则此项为该Symbol的类型
		SymbolType* subArrayType;  // 子数组类型.如果isArray==false,则此项为null
		SymbolType(bool isA, int w, std::string bt, SymbolType* sub=nullptr) 
			: isArray(isA), width(w), baseType(bt), subArrayType(sub) {}
		~SymbolType() {	
			delete subArrayType;
		}
	};
	// 单张符号表
	struct SymbolTable
	{
		SymbolTable* fatherTable;
		std::unordered_map<std::string, SymbolType*> table;
		SymbolTable(SymbolTable* father = nullptr) : fatherTable(father) {}
		~SymbolTable() {
			delete fatherTable;
			for (auto& pair : table) {
				delete pair.second;
			}
		}
		// 插入到当前的符号
		void insert(const std::string& name, SymbolType* type) {
			table[name] = type;
		}
		// 从当前符号表查找某个符号
		bool lookup(const std::string& name, SymbolType* outType) {
			auto it = table.find(name);
			if (it != table.end()) {
				outType = it->second;
				return true;
			}
			else if (fatherTable != nullptr) {
				return fatherTable->lookup(name, outType);
			}
			return false;
		}
	};
	// 管理符号表的类
	class SymbolTables
	{
	public:
		static SymbolTables& Instance();
		SymbolTable* currentTable;
		void insert(const std::string& name, SymbolType* type);
		bool lookup(const std::string& name, SymbolType* outType);
		void newZone();
		void deleteZone();
	private:
		SymbolTables();
		static SymbolTables* instance;
	};
	SymbolTables& GetSymbolTables();

	class SemanticAnalyzer
	{
	public:
		static SemanticAnalyzer& Instance();
		void SetStartWith(int num);
		int tempCount = 0;		// 临时变量计数
		int nextInStr = 0;		// 三元式计数
		int loopCounter = 0;    // 循环层数计数
		std::vector<NoteBase*> NotesFlow;	// 符号栈
		std::map<int, std::string> GenResult;  // gen输出的结果
		std::string newTemp();
		void gen(const std::string& str);
		void put(const std::string& lexeme, SymbolType* type);
		SymbolType* get(const std::string& lexeme);
		std::vector<int> makeList(int i);
		std::vector<int> merge(std::vector<int> p1, std::vector<int> p2);
		void backpatch(std::vector<int> p, int i);
		bool checkIsInLoop() const;
		SymbolType* maxType(const SymbolType* a, const SymbolType* b);
		SymbolType* createType(std::string baseType, std::stack<int> arrayIndexs);
		// error detect
		bool CheckUndefinedVariable(const std::string& lexeme);
		bool CheckTypeFit(const SymbolType* a, const SymbolType* b);
		bool CheckOutOfIndex(const SymbolType* arrayType, int index);

		void analysis();	// 就是slr分析器,不过要加一下内容
		void output();		// 输出
	private:
		static SemanticAnalyzer* instance;
	};
	SemanticAnalyzer& GetSemanticAnalyzer();

}