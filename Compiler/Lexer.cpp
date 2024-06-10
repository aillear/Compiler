#include "Lexer.h"
#include<string>
#include<cctype>
#include <fstream>
using namespace lexer;


// singleton here
Lexer* Lexer::instance = nullptr;
Lexer& Lexer::Instance() {
    if (instance == nullptr) {
        instance = new Lexer();
    }
    return *instance;
}
Lexer& lexer::GetLexer() {
    return Lexer::Instance();
}

void Lexer::SetFilePointer(FILE* _fp) {
    fp = _fp;
}

void Lexer::Readch() {
    peek = fgetc(fp);
}

bool Lexer::Readch(char c) {
    Readch();
    if (peek != c) return false;
    peek = ' ';
    return true;
}

void Lexer::Reverse(Word w) {
    words.insert(std::make_pair(w.lexeme, w));
}


Token* Lexer::Scan() {
L1:
    // 处理空行
    for (;; Readch()) {
        row++;
        if (peek == ' ' || peek == '\t') continue;
        else if (peek == '\n') {
            line++;
            row = 1;
        }
        else if (peek == EOF) return new Word("EOF", Tag::END);	// 
        else break;
    }

    
    switch (peek)
    {
    case '&':
        if (Readch('&')) return new Word("&&", Tag::LOGICAL_AND); else return new Word(std::string(1, '&'), Tag::BITWISE_AND);
    case '|':
        if (Readch('|')) return new Word("||", Tag::LOGICAL_OR); else return new Word(std::string(1, '|'), Tag::BITWISE_OR);
    case '=':
        if (Readch('=')) return new Word("==", Tag::EQUALS); else return new Word(std::string(1, '='), Tag::ASSIGN);
    case '!':
        if (Readch('=')) return new Word("!=", Tag::NOT_EQUALS); else return new Word(std::string(1, '!'), Tag::LOGICAL_NOT);
    case '<':
        if (Readch('=')) return new Word("<=", Tag::LESS_EQUAL); else return new Word(std::string(1, '<'), Tag::LESS_THAN);
    case '>':
        if (Readch('=')) return new Word(">=", Tag::GREATER_EQUAL); else return new Word(std::string(1, '>'), Tag::GREATER_THAN);

    case '+':
        if (Readch('+')) return new Word("++", Tag::INCREMENT); else return new Word(std::string(1, '+'), Tag::PLUS);
    case '-':
        if (Readch('-')) return new Word("--", Tag::DECREMENT); else return new Word(std::string(1, '-'), Tag::MINUS);
    case '*':
        Readch(); return new Word(std::string(1, '*'), Tag::MULTIPLY);
    case '/':
        if (Readch('/')) {
            while (!Readch('\n'));
            return Scan();  // bug fixed
        }
        else return new Word(std::string(1, '/'), Tag::DIVIDE);
    case '%':
        Readch(); return new Word(std::string(1, '%'), Tag::MODULO);
    case '(':
        Readch(); return new Word(std::string(1, '('), Tag::LEFT_PAREN);
    case ')':
        Readch(); return new Word(std::string(1, ')'), Tag::RIGHT_PAREN);
    case '[':
        Readch(); return new Word(std::string(1, '['), Tag::LEFT_BRACKET);
    case ']':
        Readch(); return new Word(std::string(1, ']'), Tag::RIGHT_BRACKET);
    case '{':
        Readch(); return new Word(std::string(1, '{'), Tag::LEFT_BRACE);
    case '}':
        Readch(); return new Word(std::string(1, '}'), Tag::RIGHT_BRACE);
    case ',':
        Readch(); return new Word(std::string(1, ','), Tag::COMMA);
    case ';':
        Readch(); return new Word(std::string(1, ';'), Tag::SEMICOLON);
    }

    // 数字
    if (std::isdigit(peek)) {
        // int
        int v = peek - '0';
        Readch();
        while (std::isdigit(peek))
        {
            v = 10 * v + peek - '0';
            Readch();
        }
        if (peek != '.') return new Num(v);
        // float
        double dv = v;
        double d = 10;
        while (true)
        {
            Readch();
            if (!std::isdigit(peek)) break;
            dv += (peek - '0') * 1.0 / d;
            d *= 10;
        }
        return new Real(dv);
    }

    // 字母下划线->关键字/标识符
    if (std::isalpha(peek) || peek == '_') {
        std::string str;
        str.push_back(peek);
        Readch();
        while (std::isdigit(peek) || std::isalpha(peek) || peek == '_')
        {
            str.push_back(peek);
            Readch();
        }
        auto wordsPair = words.find(str);
        // 非空代表为关键字或者之前已经遇到的标识符
        if (wordsPair != words.end()) {
            return &(wordsPair->second);
        }
        Word* w = new Word(str, Tag::IDENTIFIER);
        Reverse(*w);
        return w;
    }
    // 未定义的标识符
    char ch = peek;
    Readch();
    return new Word(std::string(1, ch), Tag::UNDEFINED);
}

Token* lexer::Lexer::FixedScan()
{
    TopToken = Scan();
    return TopToken;
}

Lexer::Lexer() { 
    Reverse(Word("if", Tag::IF));
    Reverse(Word("else", Tag::ELSE));
    Reverse(Word("while", Tag::WHILE));
    Reverse(Word("do", Tag::DO));
    Reverse(Word("break", Tag::BREAK));
    Reverse(Word("true", Tag::TRUE));
    Reverse(Word("false", Tag::FALSE));
    Reverse(Type("int", Tag::INT, 4));
    Reverse(Type("float", Tag::FLOAT, 8));
    Reverse(Type("bool", Tag::BOOL, 1));
    Reverse(Type("char", Tag::CHAR, 1));
    Reverse(Word("continue", Tag::CONTINUE));
    Reverse(Word("for", Tag::FOR));
    Reverse(Word("switch", Tag::SWITCH));
    Reverse(Word("case", Tag::CASE));
    Reverse(Word("return", Tag::RETURN));
    Reverse(Word("void", Tag::VOID));
}
Lexer::~Lexer() { ; }


Token::~Token() { ; }
std::string lexer::Token::ToString() {
    return std::string("token");
}


Num::~Num() { ; }
std::string Num::ToString() {
    return std::to_string(value);
}


Real::~Real() { ; }
std::string Real::ToString() {
    return std::to_string(value);
}


Word::~Word() { ; }
std::string Word::ToString() {
    return lexeme;
}

Type::~Type() { ; }

lexer::SymbolTable::SymbolTable()
{
    AddNewSymbol("KEYWORD", "bool");
    AddNewSymbol("KEYWORD", "break");
    AddNewSymbol("KEYWORD", "case");
    AddNewSymbol("KEYWORD", "char");
    AddNewSymbol("KEYWORD", "continue");
    AddNewSymbol("KEYWORD", "do");
    AddNewSymbol("KEYWORD", "else");
    AddNewSymbol("KEYWORD", "float");
    AddNewSymbol("KEYWORD", "for");
    AddNewSymbol("KEYWORD", "if");
    AddNewSymbol("KEYWORD", "int");
    AddNewSymbol("KEYWORD", "return");
    AddNewSymbol("KEYWORD", "switch");
    AddNewSymbol("KEYWORD", "void");
    AddNewSymbol("KEYWORD", "while");
    AddNewSymbol("KEYWORD", "true");
    AddNewSymbol("KEYWORD", "false");
}

lexer::SymbolTable::~SymbolTable()
{
}

void SymbolTable::AddNewSymbol(const std::string& type, const std::string& name) {
    // 检查是否有重复项
    for (const auto& entry : table) {
        if (entry.name == name) {
            //std::cerr << "Error: Symbol '" << name << "' already exists in the symbol table." << std::endl;
            return;
        }
    }

    // 向符号表中添加新的符号项
    SymbolEntry newEntry = { type,name };
    table.push_back(newEntry);
}

void SymbolTable::OutputTable(const std::string& path) {
    // 将符号表输出到指定路径的文件中
    std::ofstream outFile(path);
    if (!outFile.is_open()) {
        std::cerr << "Error: Unable to open file for writing: " << path << std::endl;
        return;
    }
    // 只要identifier
    // 分类符号并按照类别输出
    for (const auto& entry : table) {
        if (entry.type == "KEYWORD" ) {
            outFile << entry.type << "\t" << std::left << std::setw(10) << entry.name << std::endl;
        }
    }
    for (const auto& entry : table) {
        if (entry.type == "IDENTIFIER") {
            outFile << entry.type << "\t" << std::left << std::setw(10) << entry.name << std::endl;
        }
    }
    for (const auto& entry : table) {
        if (entry.type == "NUMBER") {
            outFile << entry.type << "\t" << std::left << std::setw(10) << entry.name << std::endl;
        }
    }

    outFile.close();

    std::cout << "Symbol table has been written to: " << path << std::endl;
}
