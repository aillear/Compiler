#pragma once
#include<iostream>
#include<vector>
#include<stack>
#include<unordered_map>
#include"Tag.h"

namespace parser {
	// 表的一行
	class TableLine
	{
	public:
		std::unordered_map<std::string, std::pair<char, int>> action_goto;
		bool GetPair(std::string v, std::pair<char, int>& p);
		TableLine(const std::vector<std::string>& fields, const std::vector<std::string>& headers);
		~TableLine();

	private:

	};
	
	// 表
	class AnalysisTable
	{
	public:
		std::vector<TableLine> table;
		AnalysisTable();
		~AnalysisTable();
		bool GetPair(int i, std::string v, std::pair<char, int> &p);
	private:

	};
	
	// 一条语法规则
	struct GrammerRule
	{
		std::string left;		// 规约时移入这玩意
		std::string right;		// 输出结果(动作)
		int count;				// 只需要知道出栈的数量,上面的right用来输出结果
		// 重载运算符,方便输出
		friend std::ostream& operator<< (std::ostream& os, const GrammerRule& g) {
			os << g.left << " -> " << g.right;
			return os;
		}
	};

	// 语法集,自动增广过了
	// 里面vector存的,index对应的就是序数
	class GrammerList
	{
	public:
		static GrammerList& Instance();
		std::vector<GrammerRule> ruleList;
		GrammerRule GetGR(int i);
	private:
		static GrammerList* instance;
		GrammerList();
		~GrammerList();
	};
	GrammerList& GetGrammerList();

	// 分析器
	class Parser
	{
	public:
		static Parser& Instance();
		std::stack<int> s;
		std::stack<std::string> token; // 这里 $ 写作 END

		bool Analysis();
	private:
		static Parser* instance;
		Parser();
		~Parser();
	};
}