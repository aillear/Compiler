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
    while (true)
    {
        Token* t = GetLexer().Scan();
        if (t->tag == Tag::END) break;
        std::cout << '<' << t->tag << ", " << t->ToString() << '>' << std::endl;
    }
    return 0;
}