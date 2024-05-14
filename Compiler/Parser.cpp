#include "Parser.h"
#include "Utils.h"
#include "Lexer.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
using namespace parser;
using namespace util;

GrammarList *GrammarList::instance = nullptr;
GrammarList &GrammarList::Instance()
{
	if (instance == nullptr)
	{
		instance = new GrammarList();
	}
	return *instance;
}
GrammarRule GrammarList::GetGR(int i)
{
	return ruleList[i];
}
void parser::GrammarList::PrintList()
{
	for (int i = 0; i < ruleList.size(); i++)
	{
		std::cout << i << " : " << ruleList[i] << "::" << ruleList[i].count << std::endl;
	}
}
GrammarList::GrammarList()
{
	std::ifstream file("exe2/grammar.txt");
	// 不进行差错处理了
	if (!file.is_open())
	{
		std::cout << "oops!" << std::endl;
	}
	std::string line;
	std::string lastLeft = "null";
	while (std::getline(file, line))
	{
		std::vector<std::string> first = Split(line, "@", Trim);
		std::string left;
		std::string rightSides;
		// 如果为 # 换行
		if (first.size() == 1)
		{
			left = lastLeft;
			rightSides = first[0];
		}
		else
		{
			left = first[0];
			rightSides = first[1];
		}
		// 对一行进行拆分
		std::vector<std::string> second = Split(rightSides, "#", Trim);
		for (std::string str : second)
		{
			GrammarRule gr;
			gr.left = left;
			gr.right = str;
			gr.count = Split(str, " ", Trim).size();
			ruleList.push_back(gr);
		}
		lastLeft = left;
	}
	file.close();

	// 增广
	GrammarRule head;
	head.left = ruleList[0].left + "\'";
	head.right = ruleList[0].left;
	head.count = 1;
	ruleList.insert(ruleList.begin(), head);
}
GrammarList::~GrammarList()
{
}

GrammarList &parser::GetGrammarList()
{
	return GrammarList::Instance();
}


bool TableLine::GetPair(std::string v, std::pair<char, int> &p)
{
	auto it = action_goto.find(v);
	if (it != action_goto.end())
	{
		p = it->second;
		return true;
	}
	return false;
}
AnalysisTable* AnalysisTable::instance = nullptr;

AnalysisTable& parser::AnalysisTable::Instance()
{
	if (instance == nullptr)
	{
		instance = new AnalysisTable();
	}
	return *instance;
}


AnalysisTable& parser::GetAnalysisTable()
{
	return AnalysisTable::Instance();
}
// 生成一行
TableLine::TableLine(const std::vector<std::string> &fields, const std::vector<std::string> &headers)
{
	bool flag = false;
	int column = 1; // 列数从1开始
	std::pair<char, int> grip;
	for (const auto &field : fields)
	{
		if (!flag) {
			flag = true;
			continue;
		}
		if (!field.empty())
		{
			int value = 0;
			int len = field.length();
			// 解析字段值
			if (field[0] == 'r' || field[0] == 's') // 终结符
			{
				grip.first = field[0]; // 字母 s or r
				for (int i = 1; i < len; i++)
				{
					value = value * 10 + field[i] - '0';
				}
				grip.second = value; // 数值
			}
			else if (field[0] == 'a') // acc
			{
				grip.first = 'a'; // a
				for (int i = 3; i < len; i++)
				{
					value = value * 10 + field[i] - '0';
				}
				grip.second = value; // 0
			}
			else// 非终结符
			{
				grip.first = 'd';
				for (int i = 0; i < len; i++)
				{
					value = value * 10 + field[i] - '0';
				}
				grip.second = value;
			}
			// 使用headers中的对应列名作为键
			action_goto[headers[column]] = std::make_pair(grip.first, grip.second);
		}
		column++;
	}
}

TableLine::~TableLine()
{
}


// 生成表,从xx文件里读
AnalysisTable::AnalysisTable()
{
	std::ifstream file("exe2/slr_test.csv", std::ios::in);
	if (!file)
	{
		std::cout << "打开文件失败" << std::endl;
		exit(1);
	}

	std::string line;
	std::getline(file, line); // 读取标题行
	std::istringstream iss(line);
	std::string col;
	std::vector<std::string> headers; // 用于存储标题行的所有字符

	// 读取并存储标题行的所有字段
	while (std::getline(iss, col, ','))
	{
		headers.push_back(col);
	}

	while (std::getline(file, line))
	{
		std::istringstream iss(line);
		std::vector<std::string> fields; // 用于存储当前行的所有字段
		std::string field;

		// 读取当前行的所有字段
		while (std::getline(iss, field, ','))
		{
			fields.push_back(field);
		}

		// 使用当前行的字段和标题行的headers来创建一个新的TableLine对象
		table.push_back(TableLine(fields, headers));
	}
}

AnalysisTable::~AnalysisTable()
{
}

// 安全的
bool AnalysisTable::GetPair(int i, std::string v, std::pair<char, int> &p)
{
	if (i >= table.size())
		return false;
	return table[i].GetPair(v, p);
}

void parser::AnalysisTable::PrintTable()
{
	int statecnt = 0;
	for (const auto& line : table)
	{
		std::cout << "State: " << statecnt++ << std::endl;
		for (const auto& pair : line.action_goto)
		{
			std::cout << "  " << pair.first << ": " << pair.second.first << pair.second.second << std::endl;
		}
	}

}

Parser *Parser::instance = nullptr;
Parser::Parser()
{
	s.push(0);
	token.push("$");
	top = " ";
	PopInputStack();
}

Parser::~Parser()
{
}

Parser &Parser::Instance()
{
	if (instance == nullptr)
	{
		instance = new Parser();
	}
	return *instance;
}

Parser& parser::GetParser()
{
	return Parser::Instance();
}

void Parser::PrintStack()
{
	std::stack<std::string> token2 = token;
	std::vector<std::string> v;
	while (!token2.empty())
	{
		v.push_back(token2.top());
		token2.pop();
	}
	std::reverse(v.begin(), v.end());
	for (std::string s : v)
	{
		std::cout << s << " ";
	}
	std::cout << std::endl;
}

// 实现那个算法here
// 从 s 读取栈顶
// 符号栈就是 token
// 从 Lexer的scan() 读取的 token的ToString() 作为输入
// 文法已经读取到GrammarList了,也是单例可以直接用
bool Parser::Analysis()
{
	int err = 0;
	bool flag = true;
	while (flag) {
		int state = s.top();
		std::string input = top;
		std::pair<char, int> p;
		if (AnalysisTable::Instance().GetPair(state, input, p))
		{
			if (p.first == 's')
			{
				PrintStack();
				s.push(p.second);
				token.push(input);
				// 读取下一个输入
				PopInputStack();
				// for test
				// std::cout << "Shift " << p.second << std::endl;
				// for prod
				std::cout << "Shift" << std::endl;
			}
			else if (p.first == 'r')
			{
				PrintStack();
				// 规约
				GrammarRule gr = GetGrammarList().GetGR(p.second);
				// 出栈
				for (int i = 0; i < gr.count; i++)
				{
					s.pop();
					token.pop();
				}
				// 新的状态
				std::pair<char, int> p2;
				GetAnalysisTable().GetPair(s.top(), gr.left, p2);
				s.push(p2.second);
				token.push(gr.left);

				std::cout << "Reduce by " << gr << std::endl;
			}
			else if (p.first == 'd')
			{
				PrintStack();
				s.push(p.second);
				std::cout << "Goto " << p.second << std::endl;
			}
			else if (p.first == 'a')
			{
				PrintStack();
				std::cout << "accept" << std::endl;
				flag = false;
			}
		}
		else // todo : 差错检测
		{
			std::cout << "error" << std::endl;
			err = 1;
			flag = false;
		}
	}
	if(err)
		return false;
	return true;
}

void parser::Parser::PopInputStack()
{
	lexer::Token* token_input = lexer::GetLexer().Scan();
	std::string input;
	if (token_input->tag == lexer::Tag::NUM)
		input = "num";
	else if (token_input->tag == lexer::Tag::REAL)
		input = "real";
	else if (token_input->tag == lexer::Tag::IDENTIFIER)
		input = "id";
	else if (token_input->tag == lexer::Tag::END)
		input = "$";
	else
		input = token_input->ToString();
	top = input;
}
