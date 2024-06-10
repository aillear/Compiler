#pragma once
#include<vector>
#include<iostream>
#include<unordered_map>

namespace util {
	// split
	// 第三个参数可选,对字符串进行处理
	std::vector<std::string> Split(const std::string& str, std::string ch = " ", std::string(*func)(const std::string&) = nullptr);
	// 字符串去空格
	std::string Trim(const std::string& str);

	std::vector<std::vector<std::string>> ReadCSV(const std::string& filename);
	void slrTransform();
}