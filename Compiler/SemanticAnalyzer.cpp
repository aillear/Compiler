#include "SemanticAnalyzer.h"
#include <format>
using namespace semanticAnalyzer;


semanticAnalyzer::Terminal::Terminal()
{
}


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
	newZone();
}

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
}
void semanticAnalyzer::SemanticAnalyzer::output()
{
	for (auto it = GenResult.begin(); it != GenResult.end(); ++it) {
		std::cout << it->first << ": " << it->second << std::endl;
	}
}
SemanticAnalyzer& semanticAnalyzer::GetSemanticAnalyzer()
{
	return SemanticAnalyzer::Instance();
}