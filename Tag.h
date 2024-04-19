#pragma once
namespace lexer {
    enum Tag {
        // 
        UNDEFINED = 0,
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
        BITWISE_XOR, // ^
        BITWISE_NOT, // ~
        LEFT_SHIFT,  // <<
        RIGHT_SHIFT, // >>
        ASSIGN,      // =
        INCREMENT,   // ++
        DECREMENT,   // --
        CONDITIONAL, // ? :
        DOT,         // .
        ARROW,       // ->
        ADDRESS_OF,  // &
        DEREFERENCE, // *
        ARRAY_INDEX, // []
        COMMA,       // ,
        NUM,         // 数字常量
        REAL,        // 浮点常量
        IDENTIFIER,  // id
        BASIC
    };
}