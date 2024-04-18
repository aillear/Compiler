#pragma once
namespace lexer {
    enum Tag {
        // 
        UNDEFINED = 0,
        AUTO,
        BREAK,
        CASE,
        CHAR,
        CONTINUE,
        DO,
        ELSE,
        ENUM,
        FLOAT,
        FOR,
        GOTO,
        IF,
        INT,
        LONG,
        RETURN,
        SHORT,
        SIGNED,
        SIZEOF,
        STATIC,
        STRUCT,
        SWITCH,
        VOID,
        WHILE,
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
        SIZEOF_OP,   // sizeof
        CAST,         // (type)
        NUM,         // 数字常量
        REAL,        // 浮点常量
        INDEX,       // id
    };
}