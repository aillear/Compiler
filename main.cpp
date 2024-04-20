#include "Lexer.h"
#pragma warning(disable:4996) // 禁用警告 4996（_CRT_SECURE_NO_WARNINGS 相关的警告）

using namespace lexer;


int main() {
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
    while (true)
    {
        Token* t = GetLexer().Scan();
        if (t->tag == Tag::END) break;
        if (t->tag == Tag::UNDEFINED) {
            std::cout << "ERROR: Undefined symbol '" << t->ToString() << "' at line: " << GetLexer().line << std::endl;
        }
        else if (t->tag == Tag::IDENTIFIER) {
            std::cout << "<id, " << t->ToString() << '>' << std::endl;
            symbolTable.AddNewSymbol("IDENTIFIER", t->ToString());
        }
        else if (t->tag == Tag::NUM || t->tag == Tag::REAL) {
            std::cout << "<num, " << t->ToString() << '>' << std::endl;
            symbolTable.AddNewSymbol("NUMBER", t->ToString());
        }
        else {
            std::cout << "<" << t->ToString() << '>' << std::endl;
        }
    }

    symbolTable.OutputTable("symbol.sym"); // 输出符号表
    return 0;
}