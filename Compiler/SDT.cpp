// SDT functions
#include "SemanticAnalyzer.h"
#include <format>
#define POP(x) for (int i = 0; i < x; ++i) NotesFlow.pop()
using namespace semanticAnalyzer;
// 统一规定：0是第一条产生式
NonTerminal* SemanticAnalyzer::Program()
{
	NonTerminal* program = new NonTerminal("program");
	NonTerminal* block = (NonTerminal*)NotesFlow.top();
	POP(5);
	backpatch((std::vector<int>*)block->param["nextlist"], nextInStr);
	return program;
}
NonTerminal* SemanticAnalyzer::Block() {
	NotesFlow.pop();
	NonTerminal* stmts = (NonTerminal*)NotesFlow.top();
	POP(3);
	NonTerminal* block = new NonTerminal("block");
	block->param["nextlist"] = stmts->param["nextlist"];
	block->param["breaklist"] = stmts->param["breaklist"];
	block->param["continuelist"] = stmts->param["continuelist"];
	return block;
}
NonTerminal* SemanticAnalyzer::Decls() {
	return new NonTerminal("decls");
}
NonTerminal* SemanticAnalyzer::Decl() {
	POP(1);
	Terminal* id = (Terminal*)NotesFlow.top();
	POP(1);
	NonTerminal* type = (NonTerminal*)NotesFlow.top();
	POP(1);
	NonTerminal* decl = new NonTerminal("decl");
	SymbolType* t = createType(*(std::string*)type->param["baseType"], id->lexval, (std::stack<int>*)type->param["arrayIndexs"]);
	put(id->lexval, t);
	return decl;
}
NonTerminal* SemanticAnalyzer::Type(int i) {
	NonTerminal* type = new NonTerminal("type");
	if (i == 0) {
		POP(1);
		Terminal* num = (Terminal*)NotesFlow.top();
		POP(2);
		NonTerminal* type1 = (NonTerminal*)NotesFlow.top();
		POP(1);
		type->param["baseType"] = type1->param["baseType"];
		type->param["arrayIndexs"] = type1->param["arrayIndexs"];
		((std::stack<int>*)type->param["arrayIndexs"])->push(std::stoi(num->lexval));
	}
	else // type -> basic
	{
		NonTerminal* basic = (NonTerminal*)NotesFlow.top();
		POP(1);
		type->param["baseType"] = basic->param["type"];
		type->param["width"] = basic->param["width"];
		type->param["arrayIndexs"] = new std::stack<int>();
	}
	return type;
}
NonTerminal* SemanticAnalyzer::Basic(int i) {
	NonTerminal* basic = new NonTerminal("basic");
	POP(1);
	if (i == 0) {
		basic->param["type"] = new std::string("float");
		basic->param["width"] = new int(8);
	}
	else {
		basic->param["type"] = new std::string("int");
		basic->param["width"] = new int(4);
	}
	return basic;
}
NonTerminal* SemanticAnalyzer::Stmts(int i) {
	NonTerminal* stmts = new NonTerminal("stmts");
	if(i == 0)
	{
		NonTerminal* stmt = (NonTerminal*)NotesFlow.top();
		POP(1);
		NonTerminal* M = (NonTerminal*)NotesFlow.top();
		POP(1);
		NonTerminal* stmts1 = (NonTerminal*)NotesFlow.top();
		POP(1);
		backpatch((std::vector<int>*)stmt->param["nextlist"], *(int*)M->param["instr"]);
		stmts->param["nextlist"] = stmt->param["nextlist"];
		stmts->param["breaklist"] = merge(*(std::vector<int>*)stmt->param["breaklist"], *(std::vector<int>*)stmts1->param["breaklist"]);
		stmts->param["continuelist"] = merge(*(std::vector<int>*)stmt->param["continuelist"], *(std::vector<int>*)stmts1->param["continuelist"]);
	}
	return stmts;
}
NonTerminal* SemanticAnalyzer::Stmt(int i) {
	NonTerminal* stmt = new NonTerminal("stmt");
	if (i == 0) // id = expr ; 
	{
		POP(1);
		NonTerminal* expr = (NonTerminal*)NotesFlow.top();
		POP(2);
		Terminal* id = (Terminal*)NotesFlow.top();
		POP(1);
		CheckUndefinedVariable(id->lexval);
		CheckTypeFit(get(id->lexval), (SymbolType*)expr->param["type"]);
		gen(std::format("{} = {}", id->lexval, *(std::string*)expr->param["addr"]));
	}
	else if (i == 1) // loc = expr ; 
	{
		POP(1);
		NonTerminal* expr = (NonTerminal*)NotesFlow.top();
		POP(2);
		NonTerminal* loc = (NonTerminal*)NotesFlow.top();
		POP(1);
		CheckTypeFit((SymbolType*)loc->param["array"], (SymbolType*)expr->param["type"]);
		gen(std::format("{}[{}] = {}", ((SymbolType*)loc->param["array"])->name, *(std::string*)loc->param["addr"] ,*(std::string*)expr->param["addr"]));
	}
	else if (i == 2) // if ( bool ) M stmt1
	{
		NonTerminal* stmt1 = (NonTerminal*)NotesFlow.top();
		POP(1);
		NonTerminal* M = (NonTerminal*)NotesFlow.top();
		POP(2);
		NonTerminal* Bool = (NonTerminal*)NotesFlow.top();
		POP(3);
		backpatch((std::vector<int>*)Bool->param["truelist"], *(int*)M->param["instr"]);
		stmt->param["nextlist"] = merge(*(std::vector<int>*)Bool->param["falselist"], *(std::vector<int>*)stmt1->param["nextlist"]);
	}
	else if (i == 3) // if ( bool ) M1 stmt1 N else M2 stmt2 
	{
		NonTerminal* stmt2 = (NonTerminal*)NotesFlow.top();
		POP(1);
		NonTerminal* M2 = (NonTerminal*)NotesFlow.top();
		POP(2);
		NonTerminal* N = (NonTerminal*)NotesFlow.top();
		POP(1);
		NonTerminal* stmt1 = (NonTerminal*)NotesFlow.top();
		POP(1);
		NonTerminal* M1 = (NonTerminal*)NotesFlow.top();
		POP(2);
		NonTerminal* Bool = (NonTerminal*)NotesFlow.top();
		POP(3);
		backpatch((std::vector<int>*)Bool->param["truelist"], *(int*)M1->param["instr"]);
		backpatch((std::vector<int>*)Bool->param["falselist"], *(int*)M2->param["instr"]);
		std::vector<int>* temp = merge(*(std::vector<int>*)stmt1->param["nextlist"], *(std::vector<int>*)N->param["nextlist"]);
		stmt->param["nextlist"] = merge(*temp, *(std::vector<int>*)stmt2->param["nextlist"]);
	}
	else if (i == 4) // while M1 ( bool ) M2 stmt1
	{
		NonTerminal* stmt1 = (NonTerminal*)NotesFlow.top();
		POP(1);
		NonTerminal* M2 = (NonTerminal*)NotesFlow.top();
		POP(2);
		NonTerminal* Bool = (NonTerminal*)NotesFlow.top();
		POP(2);
		NonTerminal* M1 = (NonTerminal*)NotesFlow.top();
		POP(2);
		stmt1->param["nextlist"] = merge(*(std::vector<int>*)stmt1->param["nextlist"], *(std::vector<int>*)M1->param["continuelist"]);
		backpatch((std::vector<int>*)stmt1->param["nextlist"], *(int*)M1->param["instr"]);
		backpatch((std::vector<int>*)Bool->param["truelist"], *(int*)M2->param["instr"]);
		stmt->param["nextlist"] = merge(*(std::vector<int>*)Bool->param["falselist"], *(std::vector<int>*)stmt1->param["breaklist"]);
		gen(std::format("goto {}", *(int*)M1->param["instr"]));
	}
	else if (i == 5) // do M1 stmt1 while M2 ( bool ) ;
	{
		POP(2);
		NonTerminal* Bool = (NonTerminal*)NotesFlow.top();
		POP(2);
		NonTerminal* M2 = (NonTerminal*)NotesFlow.top();
		POP(2);
		NonTerminal* stmt1 = (NonTerminal*)NotesFlow.top();
		POP(1);
		NonTerminal* M1 = (NonTerminal*)NotesFlow.top();
		POP(2);
		stmt1->param["nextlist"] = merge(*(std::vector<int>*)stmt1->param["nextlist"], *(std::vector<int>*)stmt1->param["continuelist"]);
		backpatch((std::vector<int>*)Bool->param["truelist"], *(int*)M1->param["instr"]);
		backpatch((std::vector<int>*)stmt1->param["nextlist"], *(int*)M2->param["instr"]);
		stmt->param["nextlist"] = merge(*(std::vector<int>*)Bool->param["falselist"], *(std::vector<int>*)stmt1->param["breaklist"]);
		gen(std::format("goto {}", *(int*)M1->param["instr"]));
	}
	else if (i == 6) // break ; 
	{
		POP(2);
		checkIsInLoop();
		stmt->param["breaklist"] = makeList(nextInStr);
		gen("goto ___");
	}
	else if (i == 7) // continue ; 
	{
		POP(2);
		checkIsInLoop();
		stmt->param["continuelist"] = makeList(nextInStr);
		gen("goto ___");
	}
	else if (i == 8) // block
	{
		NonTerminal* block = (NonTerminal*)NotesFlow.top();
		POP(1);
		stmt->param["nextlist"] = block->param["nextlist"];
		stmt->param["breaklist"] = block->param["breaklist"];
		stmt->param["continuelist"] = block->param["continuelist"];
	}
	return stmt;
}
NonTerminal* SemanticAnalyzer::Loc(int i) {
	NonTerminal* loc = new NonTerminal("loc");
	if (i == 0) // loc1 [ num ] 
	{
		POP(1);
		Terminal* num = (Terminal*)NotesFlow.top();
		POP(2);
		NonTerminal* loc1 = (NonTerminal*)NotesFlow.top();
		POP(1);
		loc->param["array"] = loc1->param["array"];
		loc->param["type"] = ((SymbolType*)loc1->param["type"])->subArrayType;
		CheckOutOfIndex((SymbolType*)loc->param["type"], std::stoi(num->lexval));
		std::string* t = newTemp();
		loc->param["addr"] = t;
		gen(std::format("{} = {} * {}", *t, num->lexval, ((SymbolType*)loc->param["type"])->width));
		gen(std::format("{} = {} + {}", *(std::string*)loc->param["addr"], *(std::string*)loc1->param["addr"], *t));
	}
	else // id [ num ] 
	{
		POP(1);
		Terminal* num = (Terminal*)NotesFlow.top();
		POP(2);
		Terminal* id = (Terminal*)NotesFlow.top();
		POP(1);
		CheckUndefinedVariable(id->lexval);
		CheckIsArray(id->lexval);
		loc->param["array"] = get(id->lexval);
		loc->param["type"] = get(id->lexval)->subArrayType;
		CheckOutOfIndex((SymbolType*)loc->param["type"], std::stoi(num->lexval));
		std::string* t = newTemp();
		loc->param["addr"] = t;
		gen(std::format("{} = {} * {}", *t, num->lexval, ((SymbolType*)loc->param["type"])->width));
	}
	return loc;
}
NonTerminal* SemanticAnalyzer::Bool(int i) {
	NonTerminal* Bool = new NonTerminal("bool");
	if (i == 0) // bool1 || M join 
	{
		NonTerminal* join = (NonTerminal*)NotesFlow.top();
		POP(1);
		NonTerminal* M = (NonTerminal*)NotesFlow.top();
		POP(2);
		NonTerminal* bool1 = (NonTerminal*)NotesFlow.top();
		POP(1);
		backpatch((std::vector<int>*)bool1->param["falselist"], *(int*)M->param["instr"]);
		Bool->param["truelist"] = merge(*(std::vector<int>*)bool1->param["truelist"], *(std::vector<int>*)join->param["truelist"]);
		Bool->param["falselist"] = join->param["falselist"];
	}
	else // join
	{
		NonTerminal* join = (NonTerminal*)NotesFlow.top();
		POP(1);
		Bool->param["truelist"] = join->param["truelist"];
		Bool->param["falselist"] = join->param["falselist"];
	}
	return Bool;
}
NonTerminal* SemanticAnalyzer::Join(int i) {
	NonTerminal* join = new NonTerminal("join");
	if (i == 0) // join1 && M equality 
	{
		NonTerminal* equality = (NonTerminal*)NotesFlow.top();
		POP(1);
		NonTerminal* M = (NonTerminal*)NotesFlow.top();
		POP(2);
		NonTerminal* join1 = (NonTerminal*)NotesFlow.top();
		POP(1);
		backpatch((std::vector<int>*)join1->param["truelist"], *(int*)M->param["instr"]);
		join->param["truelist"] = equality->param["truelist"];
		join->param["falselist"] = merge(*(std::vector<int>*)join1->param["falselist"], *(std::vector<int>*)equality->param["falselist"]);
	}
	else // equality
	{
		NonTerminal* equality = (NonTerminal*)NotesFlow.top();
		POP(1);
		join->param["truelist"] = equality->param["truelist"];
		join->param["falselist"] = equality->param["falselist"];
	}
	return join;
}
NonTerminal* SemanticAnalyzer::Equality(int i) {
	NonTerminal* equality = new NonTerminal("equality");
	if (i == 0) // expr1 rel expr2
	{
		NonTerminal* expr2 = (NonTerminal*)NotesFlow.top();
		POP(1);
		NonTerminal* rel = (NonTerminal*)NotesFlow.top();
		POP(1);
		NonTerminal* expr1 = (NonTerminal*)NotesFlow.top();
		POP(1);
		equality->param["truelist"] = makeList(nextInStr);
		equality->param["falselist"] = makeList(nextInStr + 1);
		gen(std::format("if {} {} {} goto ___", (std::string*)expr1->param["addr"], (std::string*)rel->param["op"], (std::string*)expr2->param["addr"]));
		gen("goto ___");
	}
	else if(i == 1)// true 
	{
		equality->param["truelist"] = makeList(nextInStr);
		gen("goto ___");
	}
	else if (i == 2) // false
	{
		equality->param["falselist"] = makeList(nextInStr);
		gen("goto ___");
	}
}
NonTerminal* SemanticAnalyzer::Rel(int i) {
	NonTerminal* rel = new NonTerminal("rel");
	rel->param["op"] = new std::string(NotesFlow.top()->tokenValue);
	POP(1);
	return rel;
}
NonTerminal* SemanticAnalyzer::Expr(int i) {
	NonTerminal* expr = new NonTerminal("expr");
	if (i == 0) // expr1 opa term 
	{
		NonTerminal* term = (NonTerminal*)NotesFlow.top();
		POP(1);
		NonTerminal* opa = (NonTerminal*)NotesFlow.top();
		POP(1);
		NonTerminal* expr1 = (NonTerminal*)NotesFlow.top();
		POP(1);
		expr->param["type"] = maxType((SymbolType*)expr1->param["type"], (SymbolType*)term->param["type"]);
		std::string* t = newTemp();
		expr->param["addr"] = t;
		gen(std::format("{} = {} {} {}", *t, *(std::string*)expr1->param["addr"], *(std::string*)opa->param["op"], *(std::string*)term->param["addr"]));
	}
	else // term
	{
		NonTerminal* term = (NonTerminal*)NotesFlow.top();
		POP(1);
		expr->param["type"] = term->param["type"];
		expr->param["addr"] = term->param["addr"];
	}
	return expr;
}
NonTerminal* SemanticAnalyzer::Term(int i) {
	NonTerminal* term = new NonTerminal("term");
	if (i == 0) // term1 opb unary 
	{
		NonTerminal* unary = (NonTerminal*)NotesFlow.top();
		POP(1);
		NonTerminal* opb = (NonTerminal*)NotesFlow.top();
		POP(1);
		NonTerminal* term1 = (NonTerminal*)NotesFlow.top();
		POP(1);
		term->param["type"] = maxType((SymbolType*)term1->param["type"], (SymbolType*)unary->param["type"]);
		std::string* t = newTemp();
		term->param["addr"] = t;
		gen(std::format("{} = {} {} {}", *t, *(std::string*)term1->param["addr"], *(std::string*)opb->param["op"], *(std::string*)unary->param["addr"]));
	}
	else // unary
	{
		NonTerminal* unary = (NonTerminal*)NotesFlow.top();
		POP(1);
		term->param["type"] = unary->param["type"];
		term->param["addr"] = unary->param["addr"];
	}
	return term;
}
NonTerminal* SemanticAnalyzer::Unary(int i) {
	NonTerminal* unary = new NonTerminal("unary");
	if (i == 0) // opc unary1 
	{
		NonTerminal* unary1 = (NonTerminal*)NotesFlow.top();
		POP(1);
		NonTerminal* opc = (NonTerminal*)NotesFlow.top();
		POP(1);
		unary->param["type"] = unary1->param["type"];
		std::string* t = newTemp();
		unary->param["addr"] = t;
		gen(std::format("{} = {} {}", *t, *(std::string*)opc->param["op"], *(std::string*)unary1->param["addr"]));
	}
	else // factor
	{
		NonTerminal* factor = (NonTerminal*)NotesFlow.top();
		POP(1);
		unary->param["type"] = factor->param["type"];
		unary->param["addr"] = factor->param["addr"];
	}
	return unary;
}
NonTerminal* SemanticAnalyzer::Factor(int i) {
	NonTerminal* factor = new NonTerminal("factor");
	if (i == 0) // ( expr )
	{
		POP(1);
		NonTerminal* expr = (NonTerminal*)NotesFlow.top();
		POP(2);
		factor->param["type"] = expr->param["type"];
		factor->param["addr"] = expr->param["addr"];
	}
	else if (i == 1) // loc
	{
		NonTerminal* loc = (NonTerminal*)NotesFlow.top();
		POP(1);
		std::string* t = newTemp();
		factor->param["addr"] = t;
		std::string typeStr = ((SymbolType*)loc->param["array"])->baseType;
		std::string name = ((SymbolType*)loc->param["array"])->name;
		int w = (typeStr == "float") ? 8 : 4;
		factor->param["type"] = new SymbolType(false, w, name, typeStr);
		gen(std::format("{} = {}[{}]", *t, name, *(std::string*)loc->param["addr"]));
	}
	else if (i == 2) // id
	{
		Terminal* id = (Terminal*)NotesFlow.top();
		POP(1);
		CheckUndefinedVariable(id->lexval);
		factor->param["type"] = get(id->lexval);
		factor->param["addr"] = new std::string(id->lexval);
	}
	else if (i == 3) // num
	{
		Terminal* num = (Terminal*)NotesFlow.top();
		POP(1);
		factor->param["type"] = new SymbolType(false, 4, "int", "");
		factor->param["addr"] = new std::string(num->lexval);
	}
	else if (i == 4) // real
	{
		Terminal* real = (Terminal*)NotesFlow.top();
		POP(1);
		factor->param["type"] = new SymbolType(false, 8, "float", "");
		factor->param["addr"] = new std::string(real->lexval);
	}
	return factor;
}
NonTerminal* SemanticAnalyzer::Opa(int i) {
	NonTerminal* opa = new NonTerminal("opa");
	POP(1);
	opa->param["op"] = new std::string(NotesFlow.top()->tokenValue);
	return opa;
}
NonTerminal* SemanticAnalyzer::Opb(int i) {
	NonTerminal* opb = new NonTerminal("opb");
	POP(1);
	opb->param["op"] = new std::string(NotesFlow.top()->tokenValue);
	return opb;
}
NonTerminal* SemanticAnalyzer::Opc(int i) {
	NonTerminal* opc = new NonTerminal("opc");
	POP(1);
	opc->param["op"] = new std::string(NotesFlow.top()->tokenValue);
	return opc;
}
NonTerminal* SemanticAnalyzer::M() {
	NonTerminal* M = new NonTerminal("M");
	M->param["instr"] = new int(nextInStr);
	return M;
}
NonTerminal* SemanticAnalyzer::N() {
	NonTerminal* N = new NonTerminal("N");
	N->param["nextlist"] = makeList(nextInStr);
	gen("goto ___");
	return N;
}