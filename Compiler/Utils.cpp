#include "Utils.h"
#include "Parser.h"
#include "Lexer.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

std::vector<std::string> util::Split(const std::string& str, std::string ch, std::string(*func)(const std::string&))
{
	//以 ch 为分割字符，把 cstr 分割为多个元素存到vector
	std::vector<std::string> ret;
	size_t pos = 0;
	size_t start = 0;
	while ((pos = str.find(ch, start)) != std::string::npos)
	{
		if (pos > start) {
			if (func == nullptr) {
				ret.push_back(str.substr(start, pos - start));
			}
			else {
				//std::cout << '-' << func(str.substr(start, pos - start)) << '-' << std::endl;
				ret.push_back(func(str.substr(start, pos - start)));
			}
		}
		start = pos + ch.size();
	}
	if (str.size() > start) {
		if (func == nullptr) {
			ret.push_back(str.substr(start));
		}
		else {
			//std::cout << '-' << func(str.substr(start)) << '-' << std::endl;
			ret.push_back(func(str.substr(start)));
		}
	}
	return ret;
}

std::string util::Trim(const std::string& str)
{
	size_t start = str.find_first_not_of(" \t\n\r");

	if (start == std::string::npos) {
		return "";
	}

	size_t end = str.find_last_not_of(" \t\n\r");
	return str.substr(start, end - start + 1);
}

void util::slrTransform()
{
	std::ifstream file("exe3/slr_original.csv", std::ios::in);
	/*
	* csv format:
	* 1. header line: state, nonterminal...(ACTION), $ , terminal...(GOTO)
	* 2. task: transform "Shift xx" in ACTION to "sxx", "Reduce xx" to "rxx", "Shift xx" in GOTO to "xx"
	*/
	std::string line;
	std::vector<std::string> headers;
	std::vector<std::string> terminals;
	std::vector<std::string> nonterminals;
	std::vector<std::vector<std::pair<char, int>>> action;
	std::vector<std::vector<int>> gotoTable;
	std::vector<parser::GrammarRule> grammarRules;
	grammarRules = parser::GetGrammarList().Instance().GetRules();
	// read header
	std::getline(file, line);
	headers = util::Split(line, ",", nullptr);
	for (int i = 1; i < headers.size(); i++) {
		if (headers[i] == "$") {
			break;
		}
		nonterminals.push_back(headers[i]);
	}
	for (int i = nonterminals.size() + 1; i < headers.size(); i++) {
		terminals.push_back(headers[i]);
	}
	int action_size = nonterminals.size();
	// read table
	while (std::getline(file, line)) {
		std::vector<std::string> fields = util::Split(line, ",", nullptr);
		std::vector<std::pair<char, int>> actionLine;
		std::vector<int> gotoLine;
		for (int i = 1; i < action_size; i++) {
			if (fields[i] == "") {
				actionLine.push_back(std::make_pair(' ', -1));
			}
			else if (fields[i].find("Shift") != std::string::npos) {
				actionLine.push_back(std::make_pair('s', std::stoi(fields[i].substr(6))));
			}
			else {
				std::vector<std::string> production = util::Split(fields[i], "@", nullptr);
				parser::GrammarRule prod = parser::GrammarRule(production[0], production[1], 0);
				int cnt = 0;
				for (parser::GrammarRule rule:grammarRules)
				{
					cnt++;
					if (rule == prod) {
						actionLine.push_back(std::make_pair('r', cnt));
					}
				}
			}
		}
		action.push_back(actionLine);
		for (int i = action_size + 1; i < fields.size(); i++) {
			if (fields[i] == "") {
				gotoLine.push_back(-1);
			}
			else {
				gotoLine.push_back(std::stoi(fields[i].substr(6)));
			}
		}
		gotoTable.push_back(gotoLine);
	}

	// save table to slr.csv
	std::ofstream outFile("exe3/slr.csv");
	outFile << "state,";
	for (std::string nonterminal : nonterminals) {
		outFile << nonterminal << ",";
	}
	outFile << "$,";
	for (std::string terminal : terminals) {
		outFile << terminal << ",";
	}
    outFile << std::endl;
	for (int i = 0; i < action.size(); i++) {
		outFile << i << ",";
		for (std::pair<char, int> act : action[i]) {
			if (act.second == -1) {
				outFile << ",";
			}
			else {
				outFile << act.first << act.second << ",";
			}
		}
		for (int j = 0; j < gotoTable[i].size(); j++) {
			if (gotoTable[i][j] == -1) {
				outFile << ",";
			}
			else {
				outFile << gotoTable[i][j] << ",";
			}
		}
		outFile << std::endl;
	}
	outFile.close();
}