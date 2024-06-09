#include "Lexer.h"
#include "Parser.h"
#include <fstream>
#include "Utils.h"
#pragma warning(disable:4996) // 禁用警告 4996（_CRT_SECURE_NO_WARNINGS 相关的警告）

using namespace lexer;
using namespace parser;

void static exe1() {
    FILE* fp;
    std::string fileName;
    std::cout << "input file name: " << std::endl;
    while (true)
    {
        std::cin >> fileName;
        fp = fopen(fileName.c_str(), "r");
        if (fp != NULL) break;
        std::cout << "can't open file!" << std::endl;
    }
    std::cout << "------------------------------" << std::endl;
    GetLexer().SetFilePointer(fp);
    SymbolTable symbolTable; // 创建符号表

    // 输出文件
    std::ofstream outFile("sample.out");
    if (!outFile.is_open()) {
        std::cerr << "Error: Unable to open file for writing: sample.out" << std::endl;
    }
    while (true)
    {
        
        Token* t = GetLexer().Scan();  // 获取本次的Token
        if (t->tag == Tag::END) break; 
        if (t->tag == Tag::UNDEFINED) {
            outFile << "ERROR: Undefined symbol '" << t->ToString() << "' at (" 
                << GetLexer().line << ", " << GetLexer().row << ")" << std::endl;
        }
        else if (t->tag == Tag::IDENTIFIER) {
            outFile << "<id, " << t->ToString() << '>' << std::endl;
            symbolTable.AddNewSymbol("IDENTIFIER", t->ToString());
        }
        else if (t->tag == Tag::NUM || t->tag == Tag::REAL) {
            outFile << "<num, " << t->ToString() << '>' << std::endl;
            symbolTable.AddNewSymbol("NUMBER", t->ToString());
        }
        else {
            outFile << "<" << t->ToString() << '>' << std::endl;
        }
    }
    outFile.close();
    std::cout << "Token list has been written to: sample.out" << std::endl;
    symbolTable.OutputTable("sample.sym"); // 输出符号表
    return;
}

void static exe2() {
    FILE* fp;
    std::string fileName;
    std::cout << "input file name: " << std::endl;
    while (true)
    {
        std::cin >> fileName;
        fp = fopen(fileName.c_str(), "r");
        if (fp != NULL) break;
        std::cout << "can't open file!" << std::endl;
    }
    std::cout << "------------------------------" << std::endl;
    // 输出文件
    std::ofstream outFile1(fileName + ".out");
    std::ofstream outFile2(fileName + ".err");
    if (!outFile1.is_open()) {
        std::cerr << "Error: Unable to open file for writing output file" << std::endl;
        return;
    }
    if (!outFile2.is_open()) {
        std::cerr << "Error: Unable to open file for writing err file" << std::endl;
        return;
    }

    GetLexer().SetFilePointer(fp);
    // GetAnalysisTable().PrintTable();
    // GetGrammarList().PrintList();
    if (GetParser().Analysis(outFile1, outFile2) == false) {
        outFile1 << "\nError occured, please check " << fileName << ".err" << std::endl;
        for (const auto& e : GetParser().errorList) {
            outFile2 << e << std::endl;
		}
	}
    else {
        outFile1 << "Syntax analysis completed." << std::endl;
	}
    return;
}

void static exe3() {

}

int main() {
    exe2();
    return 0;
}