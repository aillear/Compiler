#pragma once
#include<vector>
#include<iostream>

namespace util {
	// 他写的好啊
	// split
	// 第三个参数可选,对字符串进行处理
	std::vector<std::string> Split(const std::string& str, std::string ch = " ", std::string(*func)(const std::string&) = nullptr)
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

	// 字符串去空格
	std::string Trim(const std::string& str) {
		size_t start = str.find_first_not_of(" \t\n\r");

		if (start == std::string::npos) {
			return "";
		}

		size_t end = str.find_last_not_of(" \t\n\r");
		return str.substr(start, end - start + 1);
	}

}