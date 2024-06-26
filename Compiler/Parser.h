#pragma once
#include "Tag.h"
#include <iostream>
#include <stack>
#include <unordered_map>
#include <vector>

namespace parser
{
	// 表的一行
	class TableLine
	{
	public:
		std::unordered_map<std::string, std::pair<char, int>> action_goto;
		bool GetPair(std::string v, std::pair<char, int> &p);
		TableLine(const std::vector<std::string> &fields, const std::vector<std::string> &headers);
		~TableLine();

	private:
	};

	// 表
	class AnalysisTable
	{
	public:
		static AnalysisTable& Instance();
		std::vector<TableLine> table;
		std::vector<std::string> terminal;
		std::vector<std::string> nonterminal;
		AnalysisTable();
		~AnalysisTable();
		bool GetPair(int i, std::string v, std::pair<char, int> &p);
		void PrintTable();
		std::vector<std::string> GetGoto(int s);
	private:
		static AnalysisTable *instance;
	};
	AnalysisTable& GetAnalysisTable();

	// 一条语法规则
	struct GrammarRule
	{
		std::string left;  // 规约时移入这玩意
		std::string right; // 输出结果(动作)
		int count;		   // 只需要知道出栈的数量,上面的right用来输出结果
		// 重载运算符,方便输出
		friend bool operator==(const GrammarRule& g1, const GrammarRule& g2)
		{
			std::string r1 = g1.right, r2 = g2.right;
			r1.erase(std::remove(r1.begin(), r1.end(), ' '), r1.end());
			r2.erase(std::remove(r2.begin(), r2.end(), ' '), r2.end());
			return g1.left == g2.left && r1 == r2;
		}

		friend std::ostream &operator<<(std::ostream &os, const GrammarRule &g)
		{
			os << g.left << " -> " << g.right;
			return os;
		}
		GrammarRule() : left(""), right(""), count(0) {}
		GrammarRule(std::string l, std::string r, int c) : left(l), right(r), count(c) {}
	};

	// 语法集,自动增广过了
	// 里面vector存的,index对应的就是序数
	class GrammarList
	{
	public:
		static GrammarList &Instance();
		std::vector<GrammarRule> ruleList;
		GrammarRule GetGR(int i);
		void PrintList();
		std::vector<GrammarRule> GetRules() const;
	private:
		static GrammarList *instance;
		GrammarList();
		~GrammarList();
	};
	GrammarList &GetGrammarList();

	struct Error
	{
		int line, row;
		std::string msg;
		Error(int l, int r, std::string m) : line(l), row(r), msg(m) {}
		friend std::ostream& operator<<(std::ostream& os, const Error& e)
		{
			os<< "ERROR: " << e.msg << " at (" << e.line << ", " << e.row << ")";
			return os;
		}
	};

	// 分析器
	class Parser
	{
	public:
		static Parser &Instance();
		std::stack<int> s;
		std::stack<std::string> token; // 这里 $ 写作 END
		std::string top;
		std::vector<Error> errorList;
		void PrintStack(std::ofstream& fp);
		bool Analysis(std::ofstream &fp1, std::ofstream& fp2);
		void PopInputStack();
		int ErrorHandle();
	private:
		static Parser *instance;
		Parser();
		~Parser();
	};
	Parser& GetParser();

	class FollowSet
	{
	public:
		FollowSet();
		~FollowSet();
		std::unordered_map<std::string, std::vector<std::string>> followset;
		void PrintSet();
		static FollowSet &Instance();
		bool isInSet(std::string un, std::string t);
	private:
		static FollowSet *instance;
	};
	FollowSet &GetFollowSet();
}