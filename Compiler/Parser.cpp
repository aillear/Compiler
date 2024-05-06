#include "Parser.h"
#include "Utils.h"
#include<iostream>
#include<fstream>
#include<string>
using namespace parser;
using namespace util;


GrammerList* GrammerList::instance = nullptr;
GrammerList& GrammerList::Instance()
{
	if (instance == nullptr) {
		instance = new GrammerList();
	}
	return *instance;
}
GrammerRule parser::GrammerList::GetGR(int i)
{
	return ruleList[i];
}
GrammerList::GrammerList()
{
	std::ifstream file("exe2/grammar.txt");
	// 不进行差错处理了
	if (!file.is_open()) {
		std::cout << "opos!" << std::endl;
	}
	std::string line;
	std::string lastLeft = "null";
	while (std::getline(file, line))
	{
		std::vector<std::string> first = Split(line, "@", Trim);
		std::string left;
		std::string rightSides;
		// 如果为 | 换行
		if (first.size() == 1) {
			left = lastLeft;
			rightSides = first[0];
		}
		else {
			left = first[0];
			rightSides = first[1];
		}
		// 对一行进行拆分
		std::vector<std::string> second = Split(rightSides, "|", Trim);
		for (std::string str : second) {
			GrammerRule gr;
			gr.left = left;
			gr.right = str;
			gr.count = Split(rightSides).size();
			ruleList.push_back(gr);
		}
		lastLeft = left;
	}
	file.close();

	// 增广
	GrammerRule head;
	head.left = ruleList[0].left + "\'";
	head.right = ruleList[0].left;
	head.count = 1;
	ruleList.insert(ruleList.begin(), head);
}

parser::GrammerList::~GrammerList()
{
}

GrammerList& parser::GetGrammerList()
{
	return GrammerList::Instance();
}

bool parser::TableLine::GetPair(std::string v, std::pair<char, int> &p)
{
	auto it = action_goto.find(v);
	if (it != action_goto.end()) {
		p = it->second;
		return true;
	}
	return false;
}

// 生成一行
parser::TableLine::TableLine()
{
	// todo
}

parser::TableLine::~TableLine()
{
}

// 生成表,从xx文件里读
parser::AnalysisTable::AnalysisTable()
{
	// todo
}

parser::AnalysisTable::~AnalysisTable()
{
}

// 安全的
bool parser::AnalysisTable::GetPair(int i, std::string v, std::pair<char, int> &p)
{
	if (i >= table.size()) return false;
	return table[i].GetPair(v, p);
}

parser::Parser* parser::Parser::instance = nullptr;
parser::Parser::Parser()
{
	s.push(0);
	token.push("END");
}

parser::Parser::~Parser()
{
}

Parser& parser::Parser::Instance()
{
	if (instance == nullptr) {
		instance = new Parser();
	}
	return *instance;
}

bool parser::Parser::Analysis()
{
	// 实现那个算法here
	// 从 s 读取栈顶
	// 符号栈就是 token
	// 从 Lexer的scan() 读取的 token的ToString() 作为输入
	// 文法已经读取到GrammerList了,也是单例可以直接用


	return false;
}
