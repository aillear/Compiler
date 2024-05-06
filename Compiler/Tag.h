#pragma once
#include<unordered_map>
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

    std::unordered_map<Tag, std::string> tagToString = {
        {Tag::UNDEFINED, "UNDEFINED"},
        {Tag::BOOL, "BOOL"},
        {Tag::BREAK, "BREAK"},
        {Tag::CASE, "CASE"},
        {Tag::CHAR, "CHAR"},
        {Tag::CONTINUE, "CONTINUE"},
        {Tag::DO, "DO"},
        {Tag::ELSE, "ELSE"},
        {Tag::FLOAT, "FLOAT"},
        {Tag::FOR, "FOR"},
        {Tag::IF, "IF"},
        {Tag::INT, "INT"},
        {Tag::RETURN, "RETURN"},
        {Tag::SWITCH, "SWITCH"},
        {Tag::VOID, "VOID"},
        {Tag::WHILE, "WHILE"},
        {Tag::TRUE, "TRUE"},
        {Tag::FALSE, "FALSE"},
        {Tag::PLUS, "PLUS"},
        {Tag::MINUS, "MINUS"},
        {Tag::MULTIPLY, "MULTIPLY"},
        {Tag::DIVIDE, "DIVIDE"},
        {Tag::MODULO, "MODULO"},
        {Tag::EQUALS, "EQUALS"},
        {Tag::NOT_EQUALS, "NOT_EQUALS"},
        {Tag::LESS_THAN, "LESS_THAN"},
        {Tag::GREATER_THAN, "GREATER_THAN"},
        {Tag::LESS_EQUAL, "LESS_EQUAL"},
        {Tag::GREATER_EQUAL, "GREATER_EQUAL"},
        {Tag::LOGICAL_AND, "LOGICAL_AND"},
        {Tag::LOGICAL_OR, "LOGICAL_OR"},
        {Tag::LOGICAL_NOT, "LOGICAL_NOT"},
        {Tag::BITWISE_AND, "BITWISE_AND"},
        {Tag::BITWISE_OR, "BITWISE_OR"},
        {Tag::ASSIGN, "ASSIGN"},
        {Tag::INCREMENT, "INCREMENT"},
        {Tag::DECREMENT, "DECREMENT"},
        {Tag::DOT, "DOT"},
        {Tag::LEFT_PAREN, "LEFT_PAREN"},
        {Tag::RIGHT_PAREN, "RIGHT_PAREN"},
        {Tag::LEFT_BRACKET, "LEFT_BRACKET"},
        {Tag::RIGHT_BRACKET, "RIGHT_BRACKET"},
        {Tag::LEFT_BRACE, "LEFT_BRACE"},
        {Tag::RIGHT_BRACE, "RIGHT_BRACE"},
        {Tag::COMMA, "COMMA"},
        {Tag::SEMICOLON, "SEMICOLON"},
        {Tag::NUM, "NUM"},
        {Tag::REAL, "REAL"},
        {Tag::IDENTIFIER, "IDENTIFIER"},
        {Tag::END, "END"},
        {Tag::KEYWORD, "KEYWORD"}
    };

    // tag 转为 str
    std::string TagToString(Tag t) {
        return tagToString[t];
    }
}