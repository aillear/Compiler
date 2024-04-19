#pragma once
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
    };
}