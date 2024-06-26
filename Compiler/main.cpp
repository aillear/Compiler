#include "Lexer.h"
#include "Parser.h"
#include "SemanticAnalyzer.h"
#include "Utils.h"
#include <format>
#include <fstream>
#pragma warning(disable : 4996) // 禁用警告 4996（_CRT_SECURE_NO_WARNINGS 相关的警告）

using namespace lexer;
using namespace parser;
using namespace semanticAnalyzer;
void static exe1()
{
    FILE *fp;
    std::string fileName;
    std::cout << "input file name: " << std::endl;
    while (true)
    {
        std::cin >> fileName;
        fp = fopen(fileName.c_str(), "r");
        if (fp != NULL)
            break;
        std::cout << "can't open file!" << std::endl;
    }
    std::cout << "------------------------------" << std::endl;
    GetLexer().SetFilePointer(fp);
    OldSymbolTable symbolTable; // 创建符号表

    // 输出文件
    std::ofstream outFile("sample.out");
    if (!outFile.is_open())
    {
        std::cerr << "Error: Unable to open file for writing: sample.out" << std::endl;
    }
    while (true)
    {
        Token *t = GetLexer().Scan(); // 获取本次的Token
        if (t->tag == Tag::END)
            break;
        if (t->tag == Tag::UNDEFINED)
        {
            outFile << "ERROR: Undefined symbol '" << t->ToString() << "' at ("
                    << GetLexer().line << ", " << GetLexer().row << ")" << std::endl;
        }
        else if (t->tag == Tag::IDENTIFIER)
        {
            outFile << "<id, " << t->ToString() << '>' << std::endl;
            symbolTable.AddNewSymbol("IDENTIFIER", t->ToString());
        }
        else if (t->tag == Tag::NUM || t->tag == Tag::REAL)
        {
            outFile << "<num, " << t->ToString() << '>' << std::endl;
            symbolTable.AddNewSymbol("NUMBER", t->ToString());
        }
        else
        {
            outFile << "<" << t->ToString() << '>' << std::endl;
        }
    }
    outFile.close();
    std::cout << "Token list has been written to: sample.out" << std::endl;
    symbolTable.OutputTable("sample.sym"); // 输出符号表
    return;
}

void static exe2()
{
    FILE *fp;
    std::string fileName;
    std::cout << "input file name: " << std::endl;
    while (true)
    {
        std::cin >> fileName;
        fp = fopen(fileName.c_str(), "r");
        if (fp != NULL)
            break;
        std::cout << "can't open file!" << std::endl;
    }
    std::cout << "------------------------------" << std::endl;
    // 输出文件
    std::ofstream outFile1(fileName + ".out");
    std::ofstream outFile2(fileName + ".err");
    if (!outFile1.is_open())
    {
        std::cerr << "Error: Unable to open file for writing output file" << std::endl;
        return;
    }
    if (!outFile2.is_open())
    {
        std::cerr << "Error: Unable to open file for writing err file" << std::endl;
        return;
    }
    GetLexer().SetFilePointer(fp);
    // GetAnalysisTable().PrintTable();
    // GetGrammarList().PrintList();
    if (GetParser().Analysis(outFile1, outFile2) == false)
    {
        outFile1 << "\nError occured in grammar analysis, please check " << fileName << ".err" << std::endl;
        for (const auto &e : GetParser().errorList)
        {
            outFile2 << e << std::endl;
        }
        std::cout << "Analysis list has been written to: " << fileName << ".out" << std::endl;
        std::cout << "Error list has been written to: " << fileName << ".err" << std::endl;
    }
    else
    {
        outFile1 << "Syntax analysis completed." << std::endl;
        std::cout << "Analysis list has been written to: " << fileName << ".out" << std::endl;
    }
    return;
}

void static exe3()
{
    FILE *fp;
    std::string fileName = "exe3/sample.txt";
    std::cout << "input file name: " << std::endl;
    while (true)
    {
        std::cin >> fileName;
        fp = fopen(fileName.c_str(), "r");
        if (fp != NULL)
            break;
        std::cout << "can't open file!" << std::endl;
    }
    std::string partfileName = util::Split(fileName, ".")[0];
    std::cout << "------------------------------" << std::endl;
    // 输出文件
    GetLexer().SetFilePointer(fp);
    std::ofstream outFile1 = std::ofstream("exe2/sample.out");
    std::ofstream outFile2 = std::ofstream("exe2/sample.err");
    // do grammar analysis first
    if (GetParser().Analysis(outFile1, outFile2) == false)
    {
        // if error occured, output error list and return
        std::cout << "Error occured in grammar analysis!";
        return;
    }
    outFile1 = std::ofstream("exe3/sample.out");
    std::ofstream truncateFile("exe3/sample.err", std::ios::trunc);
    truncateFile.close();
    fp = fopen(fileName.c_str(), "r");
    GetLexer().SetFilePointer(fp);
    GetSemanticAnalyzer().SetStartWith(100);
    GetSemanticAnalyzer().analysis();
    GetSemanticAnalyzer().output(outFile1);
    if (GetSemanticAnalyzer().hasError == true)
    {

        outFile1 << "\nError occured, please check " << partfileName << ".err" << std::endl;
        std::cout << "Analysis list has been written to: " << partfileName << ".out" << std::endl;
        std::cout << "Error list has been written to: " << partfileName << ".err" << std::endl;
    }
    else
    {
        outFile1 << "Semantic analysis completed." << std::endl;
        std::cout << "Analysis list has been written to: " << partfileName << ".out" << std::endl;
    }
    outFile1.close();
    return;
}

int main()
{
    exe3();
    return 0;
}