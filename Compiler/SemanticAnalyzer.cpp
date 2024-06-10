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
bool semanticAnalyzer::SymbolTables::lookup(const std::string& name, SymbolType* outType)
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
std::string semanticAnalyzer::SemanticAnalyzer::newTemp()
{
	return std::format("t{}", tempCount++);
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
std::vector<int> semanticAnalyzer::SemanticAnalyzer::makeList(int i)
{
	return std::vector<int>{i};
}
std::vector<int> semanticAnalyzer::SemanticAnalyzer::merge(std::vector<int> p1, std::vector<int> p2)
{
	std::unordered_set<int> uniqueElements;
	for (int num : p1) {
		uniqueElements.insert(num);
	}
	for (int num : p2) {
		uniqueElements.insert(num);
	}
	return std::vector<int>(uniqueElements.begin(), uniqueElements.end());
}
void semanticAnalyzer::SemanticAnalyzer::backpatch(std::vector<int> p, int i)
{
	for (int index : p) {
		GenResult[index].replace(GenResult[index].find("___"), 3, std::to_string(i));
	}
}
bool semanticAnalyzer::SemanticAnalyzer::checkIsInLoop() const
{
	return (loopCounter > 0);
}
SymbolType* semanticAnalyzer::SemanticAnalyzer::maxType(const SymbolType* a, const SymbolType* b)
{
	SymbolType* res = new SymbolType(false, 8, "float");
	if (a->baseType == b->baseType && a->baseType == "int") {
		res->width = 4;
		res->baseType = "int";
	}
	return res;
}
SymbolType* semanticAnalyzer::SemanticAnalyzer::createType(std::string baseType, std::stack<int> arrayIndexs)
{
	int w = (baseType == "int") ? 4 : 8;
	SymbolType* temp = new SymbolType(false, w, baseType);
	if (arrayIndexs.empty()) {
		return temp;
	}
	while (!arrayIndexs.empty())
	{
		w *= arrayIndexs.top();
		arrayIndexs.pop();
		temp = new SymbolType(true, w, baseType, temp);
	}
	return temp;
}
bool semanticAnalyzer::SemanticAnalyzer::CheckUndefinedVariable(const std::string& lexeme)
{
	SymbolType* gb = nullptr;
	return GetSymbolTables().lookup(lexeme, gb);
}
bool semanticAnalyzer::SemanticAnalyzer::CheckTypeFit(const SymbolType* a, const SymbolType* b)
{
	SymbolType* temp = maxType(a, b);
	std::string str = temp->baseType;
	delete temp;
	if (a->baseType != str) return false;
	return true;
}
bool semanticAnalyzer::SemanticAnalyzer::CheckOutOfIndex(const SymbolType* arrayType, int index)
{
	if (!arrayType->isArray || index < 0) {
		return false;
	}
	int elemWidth = arrayType->subArrayType->width;
	if (index >= arrayType->width / elemWidth) return false;
	return true;
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
				if (input == "while") {
					loopCounter++;
				}
				else if (input == "{") {
					GetSymbolTables().newZone();
				}
				else if (input == "}") {
					GetSymbolTables().deleteZone();
				}
				PopInputStack();
			}
			else if (p.first == 'r') {
				// 规约
				GrammarRule gr = GetGrammarList().GetGR(p.second);
				// 出栈
				//	 todo:  根据p.second来查找对应的sdt,转到对应的函数执行.设定返回值为nt如下
				NonTerminal* nt = nullptr;

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
void semanticAnalyzer::SemanticAnalyzer::output()
{
	for (auto it = GenResult.begin(); it != GenResult.end(); ++it) {
		std::cout << it->first << ": " << it->second << std::endl;
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
