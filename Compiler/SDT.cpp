// SDT functions
#include "SemanticAnalyzer.h"
#include <format>
using namespace semanticAnalyzer;
// 统一规定：0是第一条产生式
NonTerminal* SemanticAnalyzer::Program() //  basic id ( ) block
{
	NonTerminal* program = new NonTerminal("program");
	NonTerminal* block = (NonTerminal*)NotesFlow.top();
	// pop states and notes flow at the same time for 5 times
	PopStatesAndNotesFlow(5);
	// backpatch the nextlist of block
	// there is a param map in all nonterminal with the std::unordered_map<std::string, void*> format
	// so here we cast "block->param["nextlist"]"@void* to "std::vector<int>*"
	backpatch((std::vector<int>*)block->param["nextlist"], nextInStr); 
	return program;
}
NonTerminal* SemanticAnalyzer::Block() { // { decls stmts } 
	PopStatesAndNotesFlow(1);
	NonTerminal* stmts = (NonTerminal*)NotesFlow.top();
	PopStatesAndNotesFlow(3);
	NonTerminal* block = new NonTerminal("block");
	block->param["nextlist"] = stmts->param["nextlist"];
	block->param["breaklist"] = stmts->param["breaklist"];
	block->param["continuelist"] = stmts->param["continuelist"];
	return block;
}
NonTerminal* SemanticAnalyzer::Decls(int i) {
	if (i == 0) // decls decl
	{
		PopStatesAndNotesFlow(2);
	}
	return new NonTerminal("decls"); // epslion
}
NonTerminal* SemanticAnalyzer::Decl() { // type id ;
	PopStatesAndNotesFlow(1);
	Terminal* id = (Terminal*)NotesFlow.top();
	PopStatesAndNotesFlow(1);
	NonTerminal* type = (NonTerminal*)NotesFlow.top();
	PopStatesAndNotesFlow(1);
	NonTerminal* decl = new NonTerminal("decl");
	SymbolType* t = createType(*(std::string*)type->param["baseType"], id->lexval, (std::stack<int>*)type->param["arrayIndexs"]);
	put(id->lexval, t);
	return decl;
}
NonTerminal* SemanticAnalyzer::Type(int i) {
	NonTerminal* type = new NonTerminal("type");
	if (i == 0) { // type [ num ]
		PopStatesAndNotesFlow(1);
		Terminal* num = (Terminal*)NotesFlow.top();
		PopStatesAndNotesFlow(2);
		NonTerminal* type1 = (NonTerminal*)NotesFlow.top();
		PopStatesAndNotesFlow(1);
		type->param["baseType"] = type1->param["baseType"];
		type->param["arrayIndexs"] = type1->param["arrayIndexs"];
		((std::stack<int>*)type->param["arrayIndexs"])->push(std::stoi(num->lexval));
	}
	else // type -> basic
	{
		NonTerminal* basic = (NonTerminal*)NotesFlow.top();
		PopStatesAndNotesFlow(1);
		type->param["baseType"] = basic->param["type"];
		type->param["width"] = basic->param["width"];
		type->param["arrayIndexs"] = new std::stack<int>();
	}
	return type;
}
NonTerminal* SemanticAnalyzer::Basic(int i) {
	NonTerminal* basic = new NonTerminal("basic");
	PopStatesAndNotesFlow(1);
	if (i == 0) { // int
		basic->param["type"] = new std::string("float");
		basic->param["width"] = new int(8);
	}
	else { // float
		basic->param["type"] = new std::string("int");
		basic->param["width"] = new int(4);
	}
	return basic;
}
NonTerminal* SemanticAnalyzer::Stmts(int i) {
	NonTerminal* stmts = new NonTerminal("stmts");
	if(i == 0) // stmts stmt
	{
		NonTerminal* stmt = (NonTerminal*)NotesFlow.top();
		PopStatesAndNotesFlow(1);
		NonTerminal* M = (NonTerminal*)NotesFlow.top();
		PopStatesAndNotesFlow(1);
		NonTerminal* stmts1 = (NonTerminal*)NotesFlow.top();
		PopStatesAndNotesFlow(1);
		backpatch((std::vector<int>*)stmts1->param["nextlist"], *(int*)M->param["instr"]);
		stmts->param["nextlist"] = stmt->param["nextlist"];
		stmts->param["breaklist"] = merge((std::vector<int>*)stmt->param["breaklist"], (std::vector<int>*)stmts1->param["breaklist"]);
		stmts->param["continuelist"] = merge((std::vector<int>*)stmt->param["continuelist"], (std::vector<int>*)stmts1->param["continuelist"]);
	} // else : epslion
	return stmts;
}
NonTerminal* SemanticAnalyzer::Stmt(int i) {
	NonTerminal* stmt = new NonTerminal("stmt");
	if (i == 0) // id = expr ; 
	{
		PopStatesAndNotesFlow(1);
		NonTerminal* expr = (NonTerminal*)NotesFlow.top();
		PopStatesAndNotesFlow(2);
		Terminal* id = (Terminal*)NotesFlow.top();
		PopStatesAndNotesFlow(1);
		CheckUndefinedVariable(id->lexval);
		CheckIsAssignArrayAddr(get(id->lexval));
		CheckTypeFit(get(id->lexval), (SymbolType*)expr->param["type"]);
		SymbolType* t = get(id->lexval);
		if (t == nullptr) {
			t = new SymbolType(false, 4, "int", "");
		}

		if (t->baseType != ((SymbolType*)expr->param["type"])->baseType) {
			gen(std::format("{} = ({}){}", id->lexval, get(id->lexval)->baseType, *(std::string*)expr->param["addr"]));
		}
		else {
			gen(std::format("{} = {}", id->lexval, *(std::string*)expr->param["addr"]));
		}
	}
	else if (i == 1) // loc = expr ; 
	{
		PopStatesAndNotesFlow(1);
		NonTerminal* expr = (NonTerminal*)NotesFlow.top();
		PopStatesAndNotesFlow(2);
		NonTerminal* loc = (NonTerminal*)NotesFlow.top();
		PopStatesAndNotesFlow(1);
		CheckIsAssignArrayAddr((SymbolType*)loc->param["type"]);
		CheckTypeFit((SymbolType*)loc->param["type"], (SymbolType*)expr->param["type"]);
		SymbolType* t = (SymbolType*)loc->param["type"];
		if (t == nullptr) {
			t = new SymbolType(false, 4, "int", "");
		}
		if (t->baseType != ((SymbolType*)expr->param["type"])->baseType) {
			gen(std::format("{}[{}] = ({}){}", ((SymbolType*)loc->param["array"])->name, *(std::string*)loc->param["addr"],t->baseType, *(std::string*)expr->param["addr"]));
		}
		else {
			gen(std::format("{}[{}] = {}", ((SymbolType*)loc->param["array"])->name, *(std::string*)loc->param["addr"], *(std::string*)expr->param["addr"]));
		}
	}
	else if (i == 2) // if ( bool ) M stmt1
	{
		NonTerminal* stmt1 = (NonTerminal*)NotesFlow.top();
		PopStatesAndNotesFlow(1);
		NonTerminal* M = (NonTerminal*)NotesFlow.top();
		PopStatesAndNotesFlow(2);
		NonTerminal* Bool = (NonTerminal*)NotesFlow.top();
		PopStatesAndNotesFlow(3);
		backpatch((std::vector<int>*)Bool->param["truelist"], *(int*)M->param["instr"]);
		stmt->param["nextlist"] = merge((std::vector<int>*)Bool->param["falselist"], (std::vector<int>*)stmt1->param["nextlist"]);
		stmt->param["breaklist"] = stmt1->param["breaklist"];
		stmt->param["continuelist"] = stmt1->param["continuelist"];
	}
	else if (i == 3) // if ( bool ) M1 stmt1 N else M2 stmt2 
	{
		NonTerminal* stmt2 = (NonTerminal*)NotesFlow.top();
		PopStatesAndNotesFlow(1);
		NonTerminal* M2 = (NonTerminal*)NotesFlow.top();
		PopStatesAndNotesFlow(2);
		NonTerminal* N = (NonTerminal*)NotesFlow.top();
		PopStatesAndNotesFlow(1);
		NonTerminal* stmt1 = (NonTerminal*)NotesFlow.top();
		PopStatesAndNotesFlow(1);
		NonTerminal* M1 = (NonTerminal*)NotesFlow.top();
		PopStatesAndNotesFlow(2);
		NonTerminal* Bool = (NonTerminal*)NotesFlow.top();
		PopStatesAndNotesFlow(3);
		backpatch((std::vector<int>*)Bool->param["truelist"], *(int*)M1->param["instr"]);
		backpatch((std::vector<int>*)Bool->param["falselist"], *(int*)M2->param["instr"]);
		std::vector<int>* temp = merge((std::vector<int>*)stmt1->param["nextlist"], (std::vector<int>*)N->param["nextlist"]);
		stmt->param["nextlist"] = merge(temp, (std::vector<int>*)stmt2->param["nextlist"]);
		stmt->param["breaklist"] = merge((std::vector<int>*)stmt1->param["breaklist"], (std::vector<int>*)stmt2->param["breaklist"]);
		stmt->param["continuelist"] = merge((std::vector<int>*)stmt1->param["continuelist"], (std::vector<int>*)stmt2->param["continuelist"]);
	}
	else if (i == 4) // while M1 ( bool ) M2 stmt1
	{
		NonTerminal* stmt1 = (NonTerminal*)NotesFlow.top();
		PopStatesAndNotesFlow(1);
		NonTerminal* M2 = (NonTerminal*)NotesFlow.top();
		PopStatesAndNotesFlow(2);
		NonTerminal* Bool = (NonTerminal*)NotesFlow.top();
		PopStatesAndNotesFlow(2);
		NonTerminal* M1 = (NonTerminal*)NotesFlow.top();
		PopStatesAndNotesFlow(2);
		stmt1->param["nextlist"] = merge((std::vector<int>*)stmt1->param["nextlist"], (std::vector<int>*)stmt1->param["continuelist"]);
		backpatch((std::vector<int>*)stmt1->param["nextlist"], *(int*)M1->param["instr"]);
		backpatch((std::vector<int>*)Bool->param["truelist"], *(int*)M2->param["instr"]);
		stmt->param["nextlist"] = merge((std::vector<int>*)Bool->param["falselist"], (std::vector<int>*)stmt1->param["breaklist"]);
		gen(std::format("goto {}", *(int*)M1->param["instr"]));
		loopCounter--;
	}
	else if (i == 5) // do M1 stmt1 while M2 ( bool ) ;
	{
		PopStatesAndNotesFlow(2);
		NonTerminal* Bool = (NonTerminal*)NotesFlow.top();
		PopStatesAndNotesFlow(2);
		NonTerminal* M2 = (NonTerminal*)NotesFlow.top();
		PopStatesAndNotesFlow(2);
		NonTerminal* stmt1 = (NonTerminal*)NotesFlow.top();
		PopStatesAndNotesFlow(1);
		NonTerminal* M1 = (NonTerminal*)NotesFlow.top();
		PopStatesAndNotesFlow(2);
		stmt1->param["nextlist"] = merge((std::vector<int>*)stmt1->param["nextlist"], (std::vector<int>*)stmt1->param["continuelist"]);
		backpatch((std::vector<int>*)Bool->param["truelist"], *(int*)M1->param["instr"]);
		backpatch((std::vector<int>*)stmt1->param["nextlist"], *(int*)M2->param["instr"]);
		stmt->param["nextlist"] = merge((std::vector<int>*)Bool->param["falselist"], (std::vector<int>*)stmt1->param["breaklist"]);
		gen(std::format("goto {}", *(int*)M1->param["instr"]));
		loopCounter -= 2;
	}
	else if (i == 6) // break ; 
	{
		PopStatesAndNotesFlow(2);
		checkIsInLoop();
		stmt->param["breaklist"] = makeList(nextInStr);
		gen("goto ___");
	}
	else if (i == 7) // continue ; 
	{
		PopStatesAndNotesFlow(2);
		checkIsInLoop();
		stmt->param["continuelist"] = makeList(nextInStr);
		gen("goto ___");
	}
	else if (i == 8) // block
	{
		NonTerminal* block = (NonTerminal*)NotesFlow.top();
		PopStatesAndNotesFlow(1);
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
		PopStatesAndNotesFlow(1);
		Terminal* num = (Terminal*)NotesFlow.top();
		PopStatesAndNotesFlow(2);
		NonTerminal* loc1 = (NonTerminal*)NotesFlow.top();
		PopStatesAndNotesFlow(1);
		loc->param["array"] = loc1->param["array"];
		loc->param["type"] = ((SymbolType*)loc1->param["type"])->subArrayType;
		CheckOutOfIndex((SymbolType*)loc1->param["type"], std::stoi(num->lexval));
		std::string* t = newTemp();
		loc->param["addr"] = newTemp();
		gen(std::format("{} = {} * {}", *t, num->lexval, ((SymbolType*)loc->param["type"])->width));
		gen(std::format("{} = {} + {}", *(std::string*)loc->param["addr"], *(std::string*)loc1->param["addr"], *t));
	}
	else // id [ num ] 
	{
		PopStatesAndNotesFlow(1);
		Terminal* num = (Terminal*)NotesFlow.top();
		PopStatesAndNotesFlow(2);
		Terminal* id = (Terminal*)NotesFlow.top();
		PopStatesAndNotesFlow(1);
		CheckUndefinedVariable(id->lexval);
		CheckIsArray(id->lexval);
		loc->param["array"] = get(id->lexval);
		loc->param["type"] = get(id->lexval)->subArrayType;
		// 数组越界,避免报错
		int w = 0;
		if (CheckOutOfIndex((SymbolType*)loc->param["array"], std::stoi(num->lexval))) {
			w = ((SymbolType*)loc->param["type"])->width;
		}
		std::string* t = newTemp();
		loc->param["addr"] = t;
		gen(std::format("{} = {} * {}", *t, num->lexval, w));
	}
	return loc;
}
NonTerminal* SemanticAnalyzer::Bool(int i) {
	NonTerminal* Bool = new NonTerminal("bool");
	if (i == 0) // bool1 || M join 
	{
		NonTerminal* join = (NonTerminal*)NotesFlow.top();
		PopStatesAndNotesFlow(1);
		NonTerminal* M = (NonTerminal*)NotesFlow.top();
		PopStatesAndNotesFlow(2);
		NonTerminal* bool1 = (NonTerminal*)NotesFlow.top();
		PopStatesAndNotesFlow(1);
		backpatch((std::vector<int>*)bool1->param["falselist"], *(int*)M->param["instr"]);
		Bool->param["truelist"] = merge((std::vector<int>*)bool1->param["truelist"], (std::vector<int>*)join->param["truelist"]);
		Bool->param["falselist"] = join->param["falselist"];
	}
	else // join
	{
		NonTerminal* join = (NonTerminal*)NotesFlow.top();
		PopStatesAndNotesFlow(1);
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
		PopStatesAndNotesFlow(1);
		NonTerminal* M = (NonTerminal*)NotesFlow.top();
		PopStatesAndNotesFlow(2);
		NonTerminal* join1 = (NonTerminal*)NotesFlow.top();
		PopStatesAndNotesFlow(1);
		backpatch((std::vector<int>*)join1->param["truelist"], *(int*)M->param["instr"]);
		join->param["truelist"] = equality->param["truelist"];
		join->param["falselist"] = merge((std::vector<int>*)join1->param["falselist"], (std::vector<int>*)equality->param["falselist"]);
	}
	else // equality
	{
		NonTerminal* equality = (NonTerminal*)NotesFlow.top();
		PopStatesAndNotesFlow(1);
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
		PopStatesAndNotesFlow(1);
		NonTerminal* rel = (NonTerminal*)NotesFlow.top();
		PopStatesAndNotesFlow(1);
		NonTerminal* expr1 = (NonTerminal*)NotesFlow.top();
		PopStatesAndNotesFlow(1);
		equality->param["truelist"] = makeList(nextInStr);
		equality->param["falselist"] = makeList(nextInStr + 1);
		gen(std::format("if {} {} {} goto ___", *(std::string*)expr1->param["addr"], *(std::string*)rel->param["op"], *(std::string*)expr2->param["addr"]));
		gen("goto ___");
	}
	else if(i == 1)// true 
	{
		PopStatesAndNotesFlow(1);
		equality->param["truelist"] = makeList(nextInStr);
		gen("goto ___");
	}
	else if (i == 2) // false
	{
		PopStatesAndNotesFlow(1);
		equality->param["falselist"] = makeList(nextInStr);
		gen("goto ___");
	}
	return equality;
}
NonTerminal* SemanticAnalyzer::Rel(int i) {
	NonTerminal* rel = new NonTerminal("rel");
	rel->param["op"] = new std::string(NotesFlow.top()->tokenValue);
	PopStatesAndNotesFlow(1);
	return rel;
}
NonTerminal* SemanticAnalyzer::Expr(int i) {
	NonTerminal* expr = new NonTerminal("expr");
	if (i == 0) // expr1 opa term 
	{
		NonTerminal* term = (NonTerminal*)NotesFlow.top();
		PopStatesAndNotesFlow(1);
		NonTerminal* opa = (NonTerminal*)NotesFlow.top();
		PopStatesAndNotesFlow(1);
		NonTerminal* expr1 = (NonTerminal*)NotesFlow.top();
		PopStatesAndNotesFlow(1);
		expr->param["type"] = maxType((SymbolType*)expr1->param["type"], (SymbolType*)term->param["type"]);
		std::string* t = newTemp();
		expr->param["addr"] = t;
		gen(std::format("{} = {} {} {}", *t, *(std::string*)expr1->param["addr"], *(std::string*)opa->param["op"], *(std::string*)term->param["addr"]));
	}
	else // term
	{
		NonTerminal* term = (NonTerminal*)NotesFlow.top();
		PopStatesAndNotesFlow(1);
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
		PopStatesAndNotesFlow(1);
		NonTerminal* opb = (NonTerminal*)NotesFlow.top();
		PopStatesAndNotesFlow(1);
		NonTerminal* term1 = (NonTerminal*)NotesFlow.top();
		PopStatesAndNotesFlow(1);
		term->param["type"] = maxType((SymbolType*)term1->param["type"], (SymbolType*)unary->param["type"]);
		std::string* t = newTemp();
		term->param["addr"] = t;
		gen(std::format("{} = {} {} {}", *t, *(std::string*)term1->param["addr"], *(std::string*)opb->param["op"], *(std::string*)unary->param["addr"]));
	}
	else // unary
	{
		NonTerminal* unary = (NonTerminal*)NotesFlow.top();
		PopStatesAndNotesFlow(1);
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
		PopStatesAndNotesFlow(1);
		NonTerminal* opc = (NonTerminal*)NotesFlow.top();
		PopStatesAndNotesFlow(1);
		unary->param["type"] = unary1->param["type"];
		std::string* t = newTemp();
		unary->param["addr"] = t;
		gen(std::format("{} = {} {}", *t, *(std::string*)opc->param["op"], *(std::string*)unary1->param["addr"]));
	}
	else // factor
	{
		NonTerminal* factor = (NonTerminal*)NotesFlow.top();
		PopStatesAndNotesFlow(1);
		unary->param["type"] = factor->param["type"];
		unary->param["addr"] = factor->param["addr"];
	}
	return unary;
}
NonTerminal* SemanticAnalyzer::Factor(int i) {
	NonTerminal* factor = new NonTerminal("factor");
	if (i == 0) // ( expr )
	{
		PopStatesAndNotesFlow(1);
		NonTerminal* expr = (NonTerminal*)NotesFlow.top();
		PopStatesAndNotesFlow(2);
		factor->param["type"] = expr->param["type"];
		factor->param["addr"] = expr->param["addr"];
	}
	else if (i == 1) // loc
	{
		NonTerminal* loc = (NonTerminal*)NotesFlow.top();
		PopStatesAndNotesFlow(1);
		std::string* t = newTemp();
		factor->param["addr"] = t;
		std::string typeStr = ((SymbolType*)loc->param["array"])->baseType;
		std::string name = ((SymbolType*)loc->param["array"])->name;
		int w = (typeStr == "float") ? 8 : 4;
		factor->param["type"] = new SymbolType(false, w, typeStr, name);
		gen(std::format("{} = {}[{}]", *t, name, *(std::string*)loc->param["addr"]));
	}
	else if (i == 2) // id
	{
		Terminal* id = (Terminal*)NotesFlow.top();
		PopStatesAndNotesFlow(1);
		CheckUndefinedVariable(id->lexval);
		factor->param["type"] = get(id->lexval);
		factor->param["addr"] = new std::string(id->lexval);
	}
	else if (i == 3) // num
	{
		Terminal* num = (Terminal*)NotesFlow.top();
		PopStatesAndNotesFlow(1);
		factor->param["type"] = new SymbolType(false, 4, "int", "");
		factor->param["addr"] = new std::string(num->lexval);
	}
	else if (i == 4) // real
	{
		Terminal* real = (Terminal*)NotesFlow.top();
		PopStatesAndNotesFlow(1);
		factor->param["type"] = new SymbolType(false, 8, "float", "");
		factor->param["addr"] = new std::string(real->lexval);
	}
	return factor;
}
NonTerminal* SemanticAnalyzer::Opa(int i) {
	NonTerminal* opa = new NonTerminal("opa");
	PopStatesAndNotesFlow(1);
	if(i == 0)
		opa->param["op"] = new std::string("+");
	else
		opa->param["op"] = new std::string("-");
	return opa;
}
NonTerminal* SemanticAnalyzer::Opb(int i) {
	NonTerminal* opb = new NonTerminal("opb");
	PopStatesAndNotesFlow(1);
	if (i == 0)
		opb->param["op"] = new std::string("*");
	else
		opb->param["op"] = new std::string("/");
	return opb;
}
NonTerminal* SemanticAnalyzer::Opc(int i) {
	NonTerminal* opc = new NonTerminal("opc");
	PopStatesAndNotesFlow(1);
	if (i == 0)
		opc->param["op"] = new std::string("!");
	else
		opc->param["op"] = new std::string("-");
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