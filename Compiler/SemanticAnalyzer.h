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
		std::string tokenValue;
		bool isNonTerminal;
		NoteBase(bool isN, std::string tv) : isNonTerminal(isN), tokenValue(tv) {}
	};

	// 终结符
	class Terminal : public NoteBase
	{
	public:
		std::string lexval;
		Terminal(std::string le, std::string tv) : NoteBase(false, tv), lexval(lexval) { ; }
		Terminal(std::string tv) : NoteBase(false, tv), lexval("") { ; }
	private:
		  
	};

	// 非终结符
	class NonTerminal : public NoteBase {
	public:
		std::unordered_map<std::string, void*> param;
		NonTerminal(std::string tv) : NoteBase(true, tv) { ; }
	private:
	};

	struct SymbolType
	{
		// 此项为false,则该类型不为数组
		bool isArray;
		int width;		// 此类型的宽度
		std::string name;
		std::string baseType;  // 数组基类型.如果isArray==false,则此项为该Symbol的类型
		SymbolType* subArrayType;  // 子数组类型.如果isArray==false,则此项为null
		SymbolType(bool isA, int w, std::string bt, std::string n, SymbolType* sub=nullptr) 
			: isArray(isA), width(w), baseType(bt), name(n), subArrayType(sub) {}
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
		std::stack<NoteBase*> NotesFlow;	// 符号栈
		std::stack<int> states;				// 状态栈
		NoteBase* top = nullptr;
		std::map<int, std::string> GenResult;  // gen输出的结果
		void PopInputStack();
		std::string* newTemp();
		void gen(const std::string& str);
		void put(const std::string& lexeme, SymbolType* type);
		SymbolType* get(const std::string& lexeme);
		std::vector<int>* makeList(int i);
		std::vector<int>* merge(std::vector<int> p1, std::vector<int> p2);
		void backpatch(std::vector<int> *p, int i);
		bool checkIsInLoop() const;
		SymbolType* maxType(const SymbolType* a, const SymbolType* b);
		SymbolType* createType(std::string baseType, std::string name, std::stack<int>* arrayIndexsPos);
		NonTerminal* SDTHandler(int SDTnum);
		// error detect
		bool CheckUndefinedVariable(const std::string& lexeme);
		bool CheckTypeFit(const SymbolType* a, const SymbolType* b);
		bool CheckOutOfIndex(const SymbolType* arrayType, int index);
		bool CheckIsArray(const std::string& lexeme);

		void analysis();	// 就是slr分析器,不过要加一下内容
		void output();		// 输出
		SemanticAnalyzer();
	private:
		static SemanticAnalyzer* instance;
#pragma region SDTFunction
		NonTerminal* Program();
		NonTerminal* Block();
		NonTerminal* Decls();
		NonTerminal* Decl();
		NonTerminal* Type(int i);
		NonTerminal* Basic(int i);
		NonTerminal* Stmts(int i);
		NonTerminal* Stmt(int i);
		NonTerminal* Loc(int i);
		NonTerminal* Bool(int i);
		NonTerminal* Join(int i);
		NonTerminal* Equality(int i);
		NonTerminal* Rel(int i);
		NonTerminal* Expr(int i);
		NonTerminal* Term(int i);
		NonTerminal* Unary(int i);
		NonTerminal* Factor(int i);
		NonTerminal* Opa(int i);
		NonTerminal* Opb(int i);
		NonTerminal* Opc(int i);
		NonTerminal* M();
		NonTerminal* N();
#pragma endregion

	};
	SemanticAnalyzer& GetSemanticAnalyzer();

}