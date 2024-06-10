#include "Utils.h"
#include "Parser.h"
#include "Lexer.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
using namespace std;

vector<string> util::Split(const string& str, string ch, string(*func)(const string&))
{
	//以 ch 为分割字符，把 cstr 分割为多个元素存到vector
	vector<string> ret;
	size_t pos = 0;
	size_t start = 0;
	while ((pos = str.find(ch, start)) != string::npos)
	{
		if (pos > start) {
			if (func == nullptr) {
				ret.push_back(str.substr(start, pos - start));
			}
			else {
				//cout << '-' << func(str.substr(start, pos - start)) << '-' << endl;
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
			//cout << '-' << func(str.substr(start)) << '-' << endl;
			ret.push_back(func(str.substr(start)));
		}
	}
	return ret;
}

string util::Trim(const string& str)
{
	size_t start = str.find_first_not_of(" \t\n\r");

	if (start == string::npos) {
		return "";
	}

	size_t end = str.find_last_not_of(" \t\n\r");
	return str.substr(start, end - start + 1);
}
/*
* slr_original中，箭头替换成@，epsilon替换成~
* 依照grammer.txt解析，目前是exe2的版本
*/
void util::slrTransform()
{
	vector<vector<string>> data = ReadCSV("exe3/slr_original.csv");
	vector<string> header = data[0];
	int flag = 1, tcnt = 0, ntcnt = 0;
	for (int i = 0; i < header.size(); ++i)
	{
		if (flag)
			ntcnt++;
		else
			tcnt++;
		if (header[i] == "$")
		{
			flag = 0;
			continue;
		}
	}
	vector<parser::GrammarRule> rules = parser::GetGrammarList().Instance().GetRules();
	for (int i = 1; i < data.size(); ++i)
	{
		for (int j = 1; j < ntcnt; ++j)
		{
			if (data[i][j][0] == 'S')
			{
				data[i][j] = "s" + data[i][j].substr(6);
			}
			else if (data[i][j][0] == 'A')
			{
				data[i][j] = "acc";
			}
			else if (!data[i][j].empty())
			{
				vector<string> tmp = util::Split(data[i][j], "@");
				parser::GrammarRule prod = parser::GrammarRule(tmp[0], tmp[1], 0);
				for (auto k = rules.begin(); k != rules.end(); ++k)
				{
					if (*k == prod)
					{
						data[i][j] = "r" + to_string(k - rules.begin());
						break;
					}
				}
			}
		}
		for (int j = ntcnt; j < data[i].size(); ++j)
		{
			if (data[i][j][0] == 'S')
			{
				data[i][j] = data[i][j].substr(6);
			}
		}
	}
	// write to slr.csv
	ofstream file("exe3/slr.csv");
	if (!file.is_open()) throw runtime_error("Could not open file");
	for (const auto& row : data)
	{
		for (const auto& col : row)
		{
			file << col << ',';
		}
		file << endl;
	}
	file.close();
}

vector<vector<string>> util::ReadCSV(const string& filename) {
	vector<vector<string>> data;
	ifstream file(filename);
	string line;
	int flag = 0;
	if (!file.is_open()) throw runtime_error("Could not open file");
	while (getline(file, line)) {
		if (!flag)
		{
			line = line.substr(3);
			flag = 1;
		}
		stringstream ss(line);
		vector<string> row;
		string value;

		while (getline(ss, value, ',')) {
			row.push_back(value);
		}
		data.push_back(row);
	}

	file.close();
	return data;
}