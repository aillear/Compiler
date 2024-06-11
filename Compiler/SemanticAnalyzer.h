#pragma once
#include<string>
#include<iostream>
#include<vector>
#include<unordered_set>
#include<map>
#include<unordered_map>
#include<stack>
#include<fstream>

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
		Terminal(std::string tv, std::string le) : NoteBase(false, tv), lexval(le) { ; }
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
		bool isArray;	// 此项为false,则该类型不为数组
		int width;		// 此类型的宽度
		std::string name;	// 类型名
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
		SymbolTable* fatherTable; // 父符号表
		std::unordered_map<std::string, SymbolType*> table; // 符号表
		SymbolTable(SymbolTable* father = nullptr) : fatherTable(father) {}
		~SymbolTable() {
			fatherTable = nullptr;
			for (auto& pair : table) {
				delete pair.second;
			}
		}
		// 插入到当前的符号
		void insert(const std::string& name, SymbolType* type) {
			table[name] = type;
		}
		// 从当前符号表查找某个符号
		bool lookup(const std::string& name, SymbolType* &outType) {
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
		bool lookup(const std::string& name, SymbolType* &outType);
		void newZone(); // 新建作用域
		void deleteZone(); // 删除作用域
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
		NoteBase* top = nullptr;			// 符号栈顶
		bool hasError = false;				// 是否有错误
		std::map<int, std::string> GenResult;  // gen输出的结果
		void PopInputStack(); // 弹出输入栈
		void PopStatesAndNotesFlow(int i); // 弹出状态栈和符号栈
		std::string* newTemp(); // 生成新的临时变量
		void gen(const std::string& str); // 生成三元式
		void put(const std::string& lexeme, SymbolType* type); // 将符号插入符号表
		SymbolType* get(const std::string& lexeme); // 从符号表中获取符号
		std::vector<int>* makeList(int i); // 生成一个新的列表
		std::vector<int>* merge(std::vector<int>* p1, std::vector<int>* p2); // 合并两个列表
		void backpatch(std::vector<int> *p, int i); // 回填
		bool checkIsInLoop(); // 检查是否在循环中
		SymbolType* maxType(const SymbolType* a, const SymbolType* b); // 获取两个类型的最大类型
		SymbolType* createType(std::string baseType, std::string name, std::stack<int>* arrayIndexsPos); // 创建一个类型
		NonTerminal* SDTHandler(int SDTnum); // SDT处理函数
		// error detect
		bool CheckUndefinedVariable(const std::string& lexeme); // 检查未定义变量
		bool CheckTypeFit(const SymbolType* a, const SymbolType* b); // 检查类型是否匹配
		bool CheckOutOfIndex(const SymbolType* arrayType, int index); // 检查数组越界
		bool CheckIsArray(const std::string& lexeme); // 检查是否是数组
		void PrintInputStack(); // 打印输入栈
		// 语义分析
		void analysis();			
		void output(std::ostream &fp1);
		SemanticAnalyzer();
	private:
		static SemanticAnalyzer* instance;
#pragma region SDTFunction
		NonTerminal* Program();
		NonTerminal* Block();
		NonTerminal* Decls(int i);
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