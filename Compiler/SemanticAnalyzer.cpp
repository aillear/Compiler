#include "SemanticAnalyzer.h"
#include <format>
#include "Lexer.h"
#include "Parser.h"

using namespace semanticAnalyzer;
using namespace parser;
#pragma region SymbolTable

SymbolTables* SymbolTables::instance = nullptr;
SymbolTables& semanticAnalyzer::SymbolTables::Instance()
{
	if (instance == nullptr) {
		instance = new SymbolTables();
	}
	return *instance;
}
void semanticAnalyzer::SymbolTables::insert(const std::string& name, SymbolType* type)
{
	currentTable->insert(name, type);
}
bool semanticAnalyzer::SymbolTables::lookup(const std::string& name, SymbolType* &outType)
{
	return currentTable->lookup(name, outType);
}
SymbolTables& semanticAnalyzer::GetSymbolTables()
{
	return SymbolTables::Instance();
}
void semanticAnalyzer::SymbolTables::newZone()
{
	SymbolTable* temp = currentTable;
	currentTable = new SymbolTable(temp);
}
void semanticAnalyzer::SymbolTables::deleteZone()
{
	if (currentTable == nullptr) {
		// error here.
		return;
	}
	SymbolTable* temp = currentTable;
	currentTable = currentTable->fatherTable;
	delete temp;
}
SymbolTables::SymbolTables() {
	currentTable = nullptr;
}
#pragma endregion

#pragma region SemanticAnalyzer
SemanticAnalyzer* SemanticAnalyzer::instance = nullptr;
SemanticAnalyzer& semanticAnalyzer::SemanticAnalyzer::Instance()
{
	if (instance == nullptr) {
		instance = new SemanticAnalyzer();
	}
	return *instance;
}
void semanticAnalyzer::SemanticAnalyzer::SetStartWith(int num)
{
	this->nextInStr = num;
}
void semanticAnalyzer::SemanticAnalyzer::PopInputStack()
{
	if (top->tokenValue == "$") return;
	lexer::Token* token_input = lexer::GetLexer().FixedScan();
	Terminal* input = nullptr;
	if (token_input->tag == lexer::Tag::NUM) {
		input = new Terminal("num", token_input->ToString());
	}
	else if (token_input->tag == lexer::Tag::REAL) {
		input = new Terminal("real", token_input->ToString());
	}
	else if (token_input->tag == lexer::Tag::IDENTIFIER) {
		input = new Terminal("id", token_input->ToString());
	}
	else if (token_input->tag == lexer::Tag::END) {
		input = new Terminal("$", token_input->ToString());
	}
	else if (token_input->tag == lexer::Tag::UNDEFINED) {
		input = new Terminal("undefined", token_input->ToString());
	}
	else
	{
		input = new Terminal(token_input->ToString());
	}
	top = input;
}
void semanticAnalyzer::SemanticAnalyzer::PopStatesAndNotesFlow(int i)
{
	for (int j = 0; j < i; j++) {
		states.pop();
		NotesFlow.pop();
	}
}
std::string* semanticAnalyzer::SemanticAnalyzer::newTemp()
{
	return new std::string(std::format("t{}", tempCount++));
}
void semanticAnalyzer::SemanticAnalyzer::gen(const std::string& str)
{
	// save to a vector.
	GenResult[nextInStr++] = str;
}
void semanticAnalyzer::SemanticAnalyzer::put(const std::string& lexeme, SymbolType* type)
{
	GetSymbolTables().insert(lexeme, type);
}
SymbolType* semanticAnalyzer::SemanticAnalyzer::get(const std::string& lexeme)
{
	SymbolType* res = nullptr;
	GetSymbolTables().lookup(lexeme, res);
	return res; 
}
std::vector<int>* semanticAnalyzer::SemanticAnalyzer::makeList(int i)
{
	return new std::vector<int>{i};
}
std::vector<int>* semanticAnalyzer::SemanticAnalyzer::merge(std::vector<int>* p1, std::vector<int>* p2)
{	
	std::unordered_set<int> uniqueElements;
	if (p1 != nullptr) {
		for (int num : *p1) {
			uniqueElements.insert(num);
		}
	}
	if (p2 != nullptr) {
		for (int num : *p2) {
			uniqueElements.insert(num);
		}
	}
	return new std::vector<int>(uniqueElements.begin(), uniqueElements.end());
}
void semanticAnalyzer::SemanticAnalyzer::backpatch(std::vector<int>* p, int i)
{
	if (p == nullptr) {
		return;
	}
	for (int index : *p) {
		int tmp = GenResult[index].find("___");
		GenResult[index].replace(tmp, 3, std::to_string(i));
	}
}
bool semanticAnalyzer::SemanticAnalyzer::checkIsInLoop()
{
	if (loopCounter <= 0) {
		std::ofstream fp2 = std::ofstream("exe3/sample.err", std::ios::app);
		fp2 << std::format("Error: Break/Continue must be in a loop @ ({}, {})", lexer::GetLexer().line, lexer::GetLexer().row) << std::endl;
		hasError = true;
		fp2.close();
	}
	return loopCounter > 0;
}
SymbolType* semanticAnalyzer::SemanticAnalyzer::maxType(const SymbolType* a, const SymbolType* b)
{
	SymbolType* res = new SymbolType(false, 8, "float" , "");
	if (a->baseType == b->baseType && a->baseType == "int") {
		res->width = 4;
		res->baseType = "int";
	}
	return res;
}
SymbolType* semanticAnalyzer::SemanticAnalyzer::createType(std::string baseType, std::string name, std::stack<int> *arrayIndexsPos)
{
	std::stack<int> arrayIndexs = *arrayIndexsPos;
	int w = (baseType == "int") ? 4 : 8;
	SymbolType* temp = new SymbolType(false, w, baseType, name);
	if (arrayIndexs.empty() || arrayIndexsPos == nullptr) {
		return temp;
	}
	while (!arrayIndexs.empty())
	{
		w *= arrayIndexs.top();
		arrayIndexs.pop();
		temp = new SymbolType(true, w, baseType, name, temp);
	}
	return temp;
}
bool semanticAnalyzer::SemanticAnalyzer::CheckUndefinedVariable(const std::string& lexeme)
{
	SymbolType* gb = nullptr;
	bool flag = GetSymbolTables().lookup(lexeme, gb);
	if (flag == false)
	{
		std::ofstream fp2 = std::ofstream("exe3/sample.err", std::ios::app);
		fp2 << std::format("Error: Undefined variable {} @ ({}, {})", lexeme, lexer::GetLexer().line, lexer::GetLexer().row) << std::endl;
		hasError = true;
		fp2.close();
		return false;
	}
	return true;
}
bool semanticAnalyzer::SemanticAnalyzer::CheckTypeFit(const SymbolType* a, const SymbolType* b)
{
	SymbolType* temp = maxType(a, b);
	std::string str = temp->baseType;
	delete temp;
	if (a->baseType != str)
	{
		std::ofstream fp2 = std::ofstream("exe3/sample.err", std::ios::app);
		fp2 << std::format("Error: Type mismatch: {} & {} @ ({}, {})",a->baseType, b->baseType, lexer::GetLexer().line, lexer::GetLexer().row) << std::endl;
		hasError = true;
		fp2.close();
		return false;
	}
	return true;
}
bool semanticAnalyzer::SemanticAnalyzer::CheckOutOfIndex(const SymbolType* arrayType, int index)
{
	if (arrayType != NULL && arrayType->isArray && index >= 0) {
		int elemWidth = arrayType->subArrayType->width;
		if (index < arrayType->width / elemWidth)
			return true;
	}

	std::ofstream fp2 = std::ofstream("exe3/sample.err", std::ios::app);
	fp2 << std::format("Error: Array out of index @ ({}, {})", lexer::GetLexer().line, lexer::GetLexer().row) << std::endl;
	hasError = true;
	fp2.close();
	return false;
}
bool semanticAnalyzer::SemanticAnalyzer::CheckIsArray(const std::string& lexeme)
{
	if (get(lexeme)->isArray == false)
	{
		std::ofstream fp2 = std::ofstream("exe3/sample.err", std::ios::app);
		fp2 << std::format("Error: {} is not an array @ ({}, {})", lexeme, lexer::GetLexer().line, lexer::GetLexer().row) << std::endl;
		hasError = true;
		fp2.close();
		return false;
	}
	return true;
}
void semanticAnalyzer::SemanticAnalyzer::PrintInputStack()
{
	std::stack<NoteBase*> temp = NotesFlow;
	std::stack<NoteBase*> temp2;
	while (!temp.empty())
	{
		temp2.push(temp.top());
		temp.pop();
	}
	while (!temp2.empty())
	{
		std::cout << temp2.top()->tokenValue << " ";
		temp2.pop();
	}
	std::cout << std::endl;
}
void semanticAnalyzer::SemanticAnalyzer::analysis()
{
	while (true)
	{
		int state = states.top();
		std::string input = top->tokenValue;
		std::pair<char, int> p;
		if (GetAnalysisTable().GetPair(state, input, p)) {
			if (p.first == 's') {
				states.push(p.second);
				NotesFlow.push(top);
				// 循环计数&维护符号表
				if (input == "while" || input == "do") {
					loopCounter++;
				}
				else if (input == "{") {
					GetSymbolTables().newZone();
				}
				else if (input == "}") {
					GetSymbolTables().deleteZone();
				}
				PrintInputStack();
				std::cout << "Shift " << p.second << std::endl;
				std::cout << input << " " <<  state << "\n\n";
				PopInputStack();
			}
			else if (p.first == 'r') {
				// 规约
				GrammarRule gr = GetGrammarList().GetGR(p.second);
				// 出栈

				PrintInputStack();
				std::cout << "Reduce by " << gr << "\n";
				std::cout << input << " " << state << "\n\n";

				NonTerminal* nt = SDTHandler(p.second);
				
				std::pair<char, int> p2;
				GetAnalysisTable().GetPair(states.top(), gr.left, p2);
				// state
				states.push(p2.second);
				NotesFlow.push(nt);
			}
			else if (p.first == 'd') {
				states.push(p.second);
			}
			else if (p.first == 'a') {
				return;   // 分析完毕
			}
		}
	}
}
NonTerminal* semanticAnalyzer::SemanticAnalyzer::SDTHandler(int SDTnum)
{
	// todo:  根据SDTnum编号来查找对应的sdt,转到对应的函数执行.返回得到的Nonterminal的指针.
	// 之后可能要添加许多的函数.
	if (SDTnum == 1)
		return Program();
	else if (SDTnum == 2)
		return Block();
	else if (SDTnum <= 4)
		return Decls(SDTnum - 3);
	else if (SDTnum == 5)
		return Decl();
	else if (SDTnum <= 7)
		return Type(SDTnum - 6);
	else if (SDTnum <= 9)
		return Basic(SDTnum - 8);
	else if (SDTnum <= 11)
		return Stmts(SDTnum - 10);
	else if (SDTnum <= 20)
		return Stmt(SDTnum - 12);
	else if (SDTnum <= 22)
		return Loc(SDTnum - 21);
	else if (SDTnum <= 24)
		return Bool(SDTnum - 23);
	else if (SDTnum <= 26)
		return Join(SDTnum - 25);
	else if (SDTnum <= 29)
		return Equality(SDTnum - 27);
	else if (SDTnum <= 35)
		return Rel(SDTnum - 30);
	else if (SDTnum <= 37)
		return Expr(SDTnum - 36);
	else if (SDTnum <= 39)
		return Term(SDTnum - 38);
	else if (SDTnum <= 41)
		return Unary(SDTnum - 40);
	else if (SDTnum <= 46)
		return Factor(SDTnum - 42);
	else if (SDTnum <= 48)
		return Opa(SDTnum - 47);
	else if (SDTnum <= 50)
		return Opb(SDTnum - 49);
	else if (SDTnum <= 52)
		return Opc(SDTnum - 51);
	else if (SDTnum == 53)
		return M();
	else if (SDTnum == 54)
		return N();
	else
		throw "OutOfSDTNumRangeError";
}
void semanticAnalyzer::SemanticAnalyzer::output(std::ostream& fp1)
{
	for (auto it = GenResult.begin(); it != GenResult.end(); ++it) {
		fp1 << it->first << ": " << it->second << std::endl;
	}
}
semanticAnalyzer::SemanticAnalyzer::SemanticAnalyzer()
{
	states.push(0);
	NotesFlow.push(new Terminal("$"));
	top = new Terminal(" ");
	PopInputStack();
}
SemanticAnalyzer& semanticAnalyzer::GetSemanticAnalyzer()
{
	return SemanticAnalyzer::Instance();
}
#pragma endregion
