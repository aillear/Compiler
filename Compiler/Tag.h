#pragma once
#include<unordered_map>
#include<string>
namespace lexer {
    enum Tag {
        // 
        UNDEFINED = 0,
        BOOL,
        BREAK,
        CASE,
        CHAR,
        CONTINUE,
        DO,
        ELSE,
        FLOAT,
        FOR,
        IF,
        INT,
        RETURN,
        SWITCH,
        VOID,
        WHILE,
        TRUE,
        FALSE,
        PLUS,        // +
        MINUS,       // -
        MULTIPLY,    // *
        DIVIDE,      // /
        MODULO,      // %
        EQUALS,      // ==
        NOT_EQUALS,  // !=
        LESS_THAN,   // <
        GREATER_THAN,// >
        LESS_EQUAL,  // <=
        GREATER_EQUAL,// >=
        LOGICAL_AND, // &&
        LOGICAL_OR,  // ||
        LOGICAL_NOT, // !
        BITWISE_AND, // &
        BITWISE_OR,  // |
        ASSIGN,      // =
        INCREMENT,   // ++
        DECREMENT,   // --
        DOT,         // .
        LEFT_PAREN,     // (
        RIGHT_PAREN,    // )
        LEFT_BRACKET,   // [
        RIGHT_BRACKET,  // ]
        LEFT_BRACE,     // {
        RIGHT_BRACE,    // }
        COMMA,          // ,
        SEMICOLON,      // ;
        NUM,         // 数字常量
        REAL,        // 浮点常量
        IDENTIFIER,  // id
        END,         // end
        KEYWORD,     // 关键词
    };

    static std::unordered_map<lexer::Tag, std::string> tagToString = {
        {lexer::Tag::UNDEFINED, "UNDEFINED"},
        {lexer::Tag::BOOL, "BOOL"},
        {lexer::Tag::BREAK, "BREAK"},
        {lexer::Tag::CASE, "CASE"},
        {lexer::Tag::CHAR, "CHAR"},
        {lexer::Tag::CONTINUE, "CONTINUE"},
        {lexer::Tag::DO, "DO"},
        {lexer::Tag::ELSE, "ELSE"},
        {lexer::Tag::FLOAT, "FLOAT"},
        {lexer::Tag::FOR, "FOR"},
        {lexer::Tag::IF, "IF"},
        {lexer::Tag::INT, "INT"},
        {lexer::Tag::RETURN, "RETURN"},
        {lexer::Tag::SWITCH, "SWITCH"},
        {lexer::Tag::VOID, "VOID"},
        {lexer::Tag::WHILE, "WHILE"},
        {lexer::Tag::TRUE, "TRUE"},
        {lexer::Tag::FALSE, "FALSE"},
        {lexer::Tag::PLUS, "PLUS"},
        {lexer::Tag::MINUS, "MINUS"},
        {lexer::Tag::MULTIPLY, "MULTIPLY"},
        {lexer::Tag::DIVIDE, "DIVIDE"},
        {lexer::Tag::MODULO, "MODULO"},
        {lexer::Tag::EQUALS, "EQUALS"},
        {lexer::Tag::NOT_EQUALS, "NOT_EQUALS"},
        {lexer::Tag::LESS_THAN, "LESS_THAN"},
        {lexer::Tag::GREATER_THAN, "GREATER_THAN"},
        {lexer::Tag::LESS_EQUAL, "LESS_EQUAL"},
        {lexer::Tag::GREATER_EQUAL, "GREATER_EQUAL"},
        {lexer::Tag::LOGICAL_AND, "LOGICAL_AND"},
        {lexer::Tag::LOGICAL_OR, "LOGICAL_OR"},
        {lexer::Tag::LOGICAL_NOT, "LOGICAL_NOT"},
        {lexer::Tag::BITWISE_AND, "BITWISE_AND"},
        {lexer::Tag::BITWISE_OR, "BITWISE_OR"},
        {lexer::Tag::ASSIGN, "ASSIGN"},
        {lexer::Tag::INCREMENT, "INCREMENT"},
        {lexer::Tag::DECREMENT, "DECREMENT"},
        {lexer::Tag::DOT, "DOT"},
        {lexer::Tag::LEFT_PAREN, "LEFT_PAREN"},
        {lexer::Tag::RIGHT_PAREN, "RIGHT_PAREN"},
        {lexer::Tag::LEFT_BRACKET, "LEFT_BRACKET"},
        {lexer::Tag::RIGHT_BRACKET, "RIGHT_BRACKET"},
        {lexer::Tag::LEFT_BRACE, "LEFT_BRACE"},
        {lexer::Tag::RIGHT_BRACE, "RIGHT_BRACE"},
        {lexer::Tag::COMMA, "COMMA"},
        {lexer::Tag::SEMICOLON, "SEMICOLON"},
        {lexer::Tag::NUM, "NUM"},
        {lexer::Tag::REAL, "REAL"},
        {lexer::Tag::IDENTIFIER, "IDENTIFIER"},
        {lexer::Tag::END, "END"},
        {lexer::Tag::KEYWORD, "KEYWORD"}
    };

    // tag 转为 str
    inline std::string TagToString(lexer::Tag t) {
        return tagToString[t];
    }
}