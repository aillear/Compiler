#include "Parser.h"
#include "Utils.h"
#include "Lexer.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
using namespace parser;
using namespace lexer;
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
			if(gr.right == "~")
				gr.count = 0;
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
	std::ifstream file("exe2/slr.csv", std::ios::in);
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
	int flag = 0;
	for (auto s : headers)
	{
		if(flag == 0)
			terminal.push_back(s);
		else
			nonterminal.push_back(s);
		if (s == "$")
			flag = 1;
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

std::vector<std::string> parser::AnalysisTable::GetGoto(int s)
{
	std::vector<std::string> res;
	std::pair<char, int> p;
	for (auto str : nonterminal) {
		if (GetPair(s, str, p))
		{
			res.push_back(str);
		}
	}
	return res;
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



void Parser::PrintStack(std::ofstream& fp)
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
		fp << s << " ";
	}
	fp << std::endl;
	
	/*
	std::cout << "State Stack: ";
	std::stack<int> s2 = s;
	std::vector<int> v2;
	while (!s2.empty())
	{
		v2.push_back(s2.top());
		s2.pop();
	}
	std::reverse(v2.begin(), v2.end());
	for (int i : v2)
	{
		std::cout << i << " ";
	}
	std::cout << std::endl;
	*/

}

// 实现那个算法here
// 从 s 读取栈顶
// 符号栈就是 token
// 从 Lexer的scan() 读取的 token的ToString() 作为输入
// 文法已经读取到GrammarList了,也是单例可以直接用
bool Parser::Analysis(std::ofstream &outfile, std::ofstream& errfile)
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
				PrintStack(outfile);
				s.push(p.second);
				token.push(input);
				// 读取下一个输入
				PopInputStack();
				// for test
				// std::cout << "Shift " << p.second << std::endl;
				// std::cout << top << " " <<  state << "\n\n";
				// for prod
				outfile << "Shift" << "\n\n";
			}
			else if (p.first == 'r')
			{
				PrintStack(outfile);
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

				outfile << "Reduce by " << gr << "\n\n";
				// std::cout << top << " " << state << "\n\n";
			}
			else if (p.first == 'd')
			{
				// PrintStack(std::cout);
				s.push(p.second);
				// std::cout << "Goto " << p.second << std::endl;
				// std::cout << top << " " << state << "\n\n";
			}
			else if (p.first == 'a')
			{
				PrintStack(outfile);
				outfile << "accept" << std::endl;
				flag = false;
			}
		}
		else // 错误处理
		{
			errfile << "Error at (" << GetLexer().line << ", " << GetLexer().row << ")\nStack: \n";
			PrintStack(errfile);
			//std::cout << top << " " << state << "\n\n";
			err++;
			errorList.push_back(Error(GetLexer().line, GetLexer().row, "Syntax error, input symbol:" + top));
			// flag = false;
			ErrorHandle();
			if (top == "$")
			{
				errfile << "No way to handle error" << std::endl;
				flag = false;
			}
		}
	}
	if(err)
		return false;
	return true;
}

int parser::Parser::ErrorHandle()
{
	std::vector<std::string> tmp;
	s.pop();
	while (!s.empty()) {
		int t = s.top();
		tmp = GetAnalysisTable().GetGoto(t);
		if (tmp.size() > 0)
			break;
		s.pop();
	}
	if (s.empty())
	{
		PopInputStack();
		s.push(0);
		while (!token.empty())
			token.pop();
		token.push("$");
		return 0;
	}
	while (top != "$")
	{
		PopInputStack();
		for (std::string str : tmp)
		{
			if (GetFollowSet().isInSet(str, top)) {
				std::pair<char, int> p;
				GetAnalysisTable().GetPair(s.top(), str, p);
				s.push(p.second);
				return 1;
			}
		}
	}
	return 0;
}

void parser::Parser::PopInputStack()
{
	if (top == "$")
		return;
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




parser::FollowSet *parser::FollowSet::instance = nullptr;
parser::FollowSet::FollowSet()
{
	std::ifstream file("exe2/follow.txt");
	if (!file)
	{
		std::cout << "打开文件失败" << std::endl;
		exit(1);
	}
	std::string line;
	while (std::getline(file, line))
	{
		std::vector<std::string> first = Split(line, "@", Trim);
		std::string left = first[0];
		std::vector<std::string> second = Split(first[1], " ", Trim);
		for (std::string str : second)
		{
			followset[left].push_back(str);
		}
	}
	file.close();
}

parser::FollowSet::~FollowSet()
{
}

void parser::FollowSet::PrintSet()
{
	for (const auto& pair : followset)
	{
		std::cout << pair.first << ": ";
		for (const auto& str : pair.second)
		{
			std::cout << str << ", ";
		}
		std::cout << std::endl;
	}
}

FollowSet& parser::FollowSet::Instance()
{
	if (instance == nullptr)
	{
		instance = new FollowSet();
	}
	return *instance;
}

bool parser::FollowSet::isInSet(std::string un, std::string t)
{
	auto it = followset.find(un);
	if (it != followset.end())
	{
		auto it2 = std::find(it->second.begin(), it->second.end(), t);
		if (it2 != it->second.end())
			return true;
	}
	return false;
}

FollowSet& parser::GetFollowSet()
{
	return FollowSet::Instance();
}
