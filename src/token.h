#ifndef __TOKEN_H__
#define __TOKEN_H__

#include <stdint.h>


/* Token kinds - a kind covers a category of types. */
enum
{
/*
  TOK_COMMENT,
  TOK_PREPROCESSOR,
*/
  TOK_EOF,
  TOK_CONST,
  TOK_OPERATOR,
  TOK_KEYWORD,
  TOK_IDENTIFIER,
};



/* Token types */
enum
{
  OP_ASSIGN_LSH,      /*  0 : <<= */
  OP_ASSIGN_RSH,      /*  1 : >>= */
  OP_LSH,             /*  2 : <<  */
  OP_RSH,             /*  3 : >>  */
  OP_NOT_EQUAL,       /*  4 : !=  */
  OP_EQUAL,           /*  5 : ==  */
  OP_ASSIGN_PLUS,     /*  6 : +=  */
  OP_ASSIGN_MINUS,    /*  7 : -=  */
  OP_ASSIGN_MULTIPLY, /*  8 : *=  */
  OP_ASSIGN_DIVIDE,   /*  9 : /=  */
  OP_ASSIGN_MODULO,   /* 10 : /=  */
  OP_ASSIGN_AND,      /* 11 : &=  */
  OP_ASSIGN_OR,       /* 12 : |=  */
  OP_ASSIGN_XOR,      /* 13 : ^=  */
  OP_INCREMENT,       /* 14 : ++  */
  OP_DECREMENT,       /* 15 : --  */
  OP_LOGICAL_AND,     /* 16 : &&  */
  OP_LOGICAL_OR,      /* 17 : ||  */
  OP_ARROW,           /* 18 : ->  */
  OP_LOGICAL_GTE,     /* 19 : >=  */
  OP_LOGICAL_LTE,     /* 20 : <=  */
  OP_LOGICAL_NOT,     /* 21 : !   */
  OP_BITWISE_NOT,     /* 22 : ~   */
  OP_SEMICOLON,       /* 23 : ;   */
  OP_COLON,           /* 24 : :   */
  OP_QUESTIONMARK,    /* 25 : ?   */
  OP_COMMA,           /* 26 : ,   */
  OP_DOT,             /* 27 : .   */
  OP_PLUS,            /* 28 : +   */
  OP_MINUS,           /* 29 : -   */
  OP_MULTIPLY,        /* 30 : *   */
  OP_DIVIDE,          /* 31 : /   */
  OP_LOGICAL_GT,      /* 32 : >   */
  OP_LOGICAL_LT,      /* 33 : <   */
  OP_BITWISE_AND,     /* 34 : &   */
  OP_BITWISE_OR,      /* 35 : |   */
  OP_BITWISE_XOR,     /* 36 : ^   */
  OP_MODULO,          /* 37 : %   */
  OP_ASSIGN,          /* 38 : =   */
  OP_LPAREN,          /* 39 : (   */
  OP_RPAREN,          /* 40 : )   */
  OP_LBRACE,          /* 41 : {   */
  OP_RBRACE,          /* 42 : }   */
  OP_LBRACKET,        /* 43 : [   */
  OP_RBRACKET,        /* 44 : ]   */

  /* The 32 reserved keywords of C: */
  KW_AUTO,            /* 45 : auto     */
  KW_BREAK,           /* 46 : break    */
  KW_CASE,            /* 47 : case     */
  KW_CHAR,            /* 48 : char     */
  KW_CONST,           /* 49 : const    */
  KW_CONTINUE,        /* 50 : continue */
  KW_DEFAULT,         /* 51 : default  */
  KW_DO,              /* 52 : do       */
  KW_DOUBLE,          /* 53 : double   */
  KW_ELSE,            /* 54 : else     */
  KW_ENUM,            /* 55 : enum     */
  KW_EXTERN,          /* 56 : extern   */
  KW_FLOAT,           /* 57 : float    */
  KW_FOR,             /* 58 : for      */
  KW_GOTO,            /* 59 : goto     */
  KW_IF,              /* 60 : if       */
  KW_INT,             /* 61 : int      */
  KW_LONG,            /* 62 : long     */
  KW_REGISTER,        /* 63 : register */
  KW_RETURN,          /* 64 : return   */
  KW_SHORT,           /* 65 : short    */
  KW_SIGNED,          /* 66 : signed   */
  KW_SIZEOF,          /* 67 : sizeof   */
  KW_STATIC,          /* 68 : static   */
  KW_STRUCT,          /* 69 : struct   */
  KW_SWITCH,          /* 70 : switch   */
  KW_TYPEDEF,         /* 71 : typedef  */
  KW_UNION,           /* 72 : union    */
  KW_UNSIGNED,        /* 73 : unsigned */
  KW_VOID,            /* 74 : void     */
  KW_VOLATILE,        /* 75 : volatile */
  KW_WHILE,           /* 76 : while    */

  CNST_CHAR,          /* 77 : const char      */
  CNST_STRING,        /* 78 : string literal  */
  CNST_INT,           /* 79 : integer numeral */
  CNST_FLOAT,         /* 80 : float numeral   */
};




/* Data structure defining tokens/lexemes that we want the lexer to match. */
struct token
{
  char*    symbol;   /* string symbol, e.g. "+" for the plus operator, or "goto" etc. */
  uint32_t symlen;   /* length of above string excluding null-termination. */
  uint32_t tokknd;   /* overall lexeme type differentiator. */
  uint32_t toktyp;   /* e.g. tokknd == TOK_OPERATOR && toktyp == OP_LSH. */
  uint32_t lineno;   /* line in buffer where token was lexed. First line is no. 1. */
  uint32_t byteno;   /* byte offset into line where token was lexed. First byte is no. 0. */
  uint32_t foffset;  /* byte offset into source file, for faster lookup. */
};




#endif /* __TOKEN_H__ */

