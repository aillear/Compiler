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
		AnalysisTable();
		~AnalysisTable();
		bool GetPair(int i, std::string v, std::pair<char, int> &p);
		void PrintTable();

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
		friend std::ostream &operator<<(std::ostream &os, const GrammarRule &g)
		{
			os << g.left << " -> " << g.right;
			return os;
		}
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
	private:
		static GrammarList *instance;
		GrammarList();
		~GrammarList();
	};
	GrammarList &GetGrammarList();

	// 分析器
	class Parser
	{
	public:
		static Parser &Instance();
		std::stack<int> s;
		std::stack<std::string> token; // 这里 $ 写作 END
		std::string top;
		void PrintStack();
		bool Analysis();
		void PopInputStack();

	private:
		static Parser *instance;
		Parser();
		~Parser();
	};
	Parser& GetParser();

}