/*

  A simple lexer for C-like languages
  ===================================

    Matches:
    --------
      - single- and multi-line comments
      - string literals
      - char literals
      - hex, int and float numerals
      - ANSI C89 keywords
      - ANSI C89 operators


    Work-flow:
    ----------
      - define list of reserved keywords + operators
      - read a file
      - try treating input as (in prioritized sequence):
          comment
          whitespace
          pre-processor directive
          escape-characters
          char literal
          string literal
          integer numeral
          operator (arithmeric, logical, bitwise, etc.)
          reserved keyword
          identifier / symbol


  WORK NOTES:
 =============

    - when time comes, to implement support for '#line XX "file_yy.c" ... ' etc.,
      just make the name point into the source code and use the file_path_length

    - correctly handle invalid number literals (invalid float: '1.2.3' or octal: '0128' <- only 0-7, 8 and 9 are not allowed)
    

*/


#include "lexer.h"
#include "str.h"
#include <assert.h> /* for assert            */
#include <stdlib.h> /* for exit              */
#include <stdio.h>  /* for printf + fgetc    */
#include <stdint.h> /* for intX_t            */
#include <string.h> /* for strncmp           */



static int idx_first_tok_keyword;


/* assertion function that prints out line and char number before exiting. */
static void _expect(struct lexer* l, int p, int line);
#define expect(l, p) _expect(l, p, __LINE__)

static void print_token(struct lexer* l, struct token* t);
static void emit(struct lexer* l, struct token* next_tok, int token_kind, int token_type);
static void next(struct lexer* l);
static void consume(struct lexer* l);
static void skip_comments(struct lexer* l);






void lexer_reset_state(struct lexer* l)
{
  l->buffer = 0;
  l->buffer_original = 0;
  l->token_buffer[0] = 0;
  l->token_length = 0;
  l->cur_lineno = 1;
  l->cur_byteno = 0;
}

void lexer_init(struct lexer* l)
{
  l->nkeywords = 0;
  l->continue_on_error = 0;//1;
  lexer_reset_state(l);
}

void lexer_free(struct lexer* l)
{
  (void) l;
}

void lexer_set_char_buf(struct lexer* l, char* char_buf)
{
  expect(l, char_buf != 0);

  lexer_reset_state(l);

  l->buffer = char_buf;
  l->buffer_original = char_buf;
}

void lexer_add_token(struct lexer* l, char* token_symbol, int token_strlen, int token_kind, int token_type)
{
  expect(l, l->nkeywords < (sizeof(l->lexemes) / sizeof(*l->lexemes)));

  l->lexemes[l->nkeywords].symbol = token_symbol;  /* string symbol, e.g. "+" for the plus operator, or "goto" etc. */
  l->lexemes[l->nkeywords].symlen = token_strlen;  /* length of above string excluding null-termination. */
  l->lexemes[l->nkeywords].tokknd = token_kind;    /* overall lexeme class / type differentiator. */
  l->lexemes[l->nkeywords].toktyp = token_type;    /* e.g. tokknd == TOK_OPERATOR && toktyp == OP_LSH. */
  l->nkeywords += 1;
}

struct token lexer_next_token(struct lexer* l)
{
  struct token t;

  /* Turn bytes into lexemes / tokens: */
  while (l->buffer[0] != 0)
  {
    /* Handle comments */
    skip_comments(l);

    /* Handle code */
    switch (l->buffer[0])
    {
      case 0:
      case EOF:
        t.symbol = "<EOF>";
        t.symlen = 5;
        t.tokknd = TOK_EOF;
        t.toktyp = 0;
      return t;

      /* Whitespace: ignore it. */
      case '\f': next(l); continue;
      case '\r': next(l); continue;
      case '\n': next(l); continue;
      case '\t': next(l); continue;
      case ' ':  next(l); continue;

      /* Preprocessor directives; lines starting with <'#'> : ignore them. */
      case '#':
      {
        while (    (    (l->buffer[0] != 0)
                     && (l->buffer[0] != '\n')
                     && (l->buffer[0] != EOF))
                || (l->buffer[-1] == '\\'))
        { 
          /* Handle comments: */
          //skip_comments(l);

          next(l); /* NOTE: next(l) could be changed to consume(l) and emit() added after the loop, if you wanna save preprocessor-directives. */
        }
        continue;
      }

      /* Char literals <'x'> : */
      case '\'':
      {
        next(l); /* skip <'> */
        if (l->buffer[0] == '\\') /* handle escape-characters */
        {
          consume(l);
        }
        consume(l);
        next(l); /* skip <'> */
        emit(l, &t, TOK_CONST, CNST_CHAR);
        return t;
      }

      /* String literals <"..."> : */
      case '"':
      {
        consume(l);   /* eat <"> */
        while (    (l->buffer[0] != 0)
                && (l->buffer[0] != '"'))
        {
          if (l->buffer[0] == '\\')
          {
            if (    (l->buffer[1] == '"')
                 || (l->buffer[1] == '\'')
                 || (l->buffer[1] == 'n')
                 || (l->buffer[1] == 't')
                 || (l->buffer[1] == '\\'))
            {
              consume(l);
            }
          }
          consume(l);
        }
        consume(l);   /* eat <"> */
        emit(l, &t, TOK_CONST, CNST_STRING); /* NOTE: integrity check in emit() asserting we read at least one char */
        return t; 
      }

      /* Escape characters: */
      case '\\':
      {
        if (    (l->buffer[1] == '\n')
             || (l->buffer[1] == '\r'))
        {
          next(l); /* skip <\>  */
          next(l); /* skip <\n> */
          continue;
        }

        if (l->continue_on_error == 0)
        {
          fprintf(stderr, "ERROR: unknown escape char '\\%c' at line %d:%d. \n", l->buffer[1], l->cur_lineno, l->cur_byteno);
          exit(1);
        }
        consume(l);
        continue; 
      }

      /* Everything else: Numbers, Keywords and Identifiers. */
      default:
      {
        /* Numbers */
        if (    (l->buffer[0] >= '0')
             && (l->buffer[0] <= '9'))
        {
          /* Hexadecimal */
          if (    (l->buffer[0] == '0')
               && (l->buffer[1] == 'x'))
          {
            consume(l);
            consume(l);
            while (    (    (l->buffer[0] >= '0')
                         && (l->buffer[0] <= '9'))
                    || (    (l->buffer[0] >= 'a')
                         && (l->buffer[0] <= 'f'))
                    || (    (l->buffer[0] >= 'A')
                         && (l->buffer[0] <= 'F')))
            {
              consume(l);
            }
            emit(l, &t, TOK_CONST, CNST_INT); /* NOTE: integrity check in emit() asserting we read at least one char */
            return t;
          }
          else
          {
            /* Decimal, octal or float? */
            int is_float = 0;
            while (    (l->buffer[0] == '.')
                    || (
                           (l->buffer[0] >= '0')
                        && (l->buffer[0] <= '9')))
            {
              is_float |= (l->buffer[0] == '.');
              consume(l);
            }
            emit(l, &t, TOK_CONST, is_float ? CNST_FLOAT : CNST_INT); /* NOTE: integrity check in emit() asserting we read at least one char */
            return t;
          }
        }

        /* Keywords */
        uint32_t i;
        for (i = 0; i < idx_first_tok_keyword; ++i) /* only searching for operators */
        {
          if (    (l->lexemes[i].tokknd == TOK_OPERATOR)
               && (strncmp(l->buffer, l->lexemes[i].symbol, l->lexemes[i].symlen) == 0))
          {
            uint32_t j;
            for (j = 0; j < l->lexemes[i].symlen; ++j)
            {
              consume(l);
            }
            break;
          }
        }
        /* Found a match? */
        if (i != idx_first_tok_keyword)
        {
          emit(l, &t, l->lexemes[i].tokknd, l->lexemes[i].toktyp);
          return t;
        }

        /* Id / Symbols */
        if (    (l->buffer[0] == '_')       /* Identifiers are of form '[_a-zA-Z][_a-zA-Z0-9]*' */
             || (    (l->buffer[0] >= 'a')  /* NOTE: first char cannot be a number. */
                  && (l->buffer[0] <= 'z'))
             || (    (l->buffer[0] >= 'A')
                  && (l->buffer[0] <= 'Z')))
        {
          do
          {
            consume(l);
          }
          while (    (l->buffer[0] != 0)
                  && (    (l->buffer[0] == '_')
                       || (    (l->buffer[0] >= 'a')
                            && (l->buffer[0] <= 'z'))
                       || (    (l->buffer[0] >= '0')
                            && (l->buffer[0] <= '9'))
                       || (    (l->buffer[0] >= 'A')
                            && (l->buffer[0] <= 'Z'))));
          emit(l, &t, TOK_IDENTIFIER, CNST_STRING);
          return t;
        }

        /* Integrity check - abort if input-text is not matched at this point */
        if (l->continue_on_error == 0)
        {
          fprintf(stderr, "ERROR: unknown token '%.10s'\n", l->buffer);
          exit(1);
        }
        next(l);
      }
    }
  }

  /* Hit EOF on input stream */
  t.symbol = "<EOF>";
  t.symlen = 5;
  t.tokknd = TOK_EOF;
  t.toktyp = 0;

  return t;
}



void lexer_setup_alphabet(struct lexer* l)
{
  /* ====================================== */
  /* Lexer Alphabet defined in table below: */
  /* ====================================== */


  /* Macro to define a token/lexeme to match along with its kind and type (int/enum) */
  #define ADD_TOKEN(symbol, kind, type)     lexer_add_token(l, symbol, (sizeof(symbol) - 1), kind, type)


  /* The 45 primitives / operators in C89: */
  ADD_TOKEN(">>=",        TOK_OPERATOR,   OP_ASSIGN_LSH);
  ADD_TOKEN("<<=",        TOK_OPERATOR,   OP_ASSIGN_RSH);
  ADD_TOKEN(">>",         TOK_OPERATOR,   OP_LSH);
  ADD_TOKEN("<<",         TOK_OPERATOR,   OP_RSH);
  ADD_TOKEN("!=",         TOK_OPERATOR,   OP_NOT_EQUAL);
  ADD_TOKEN("==",         TOK_OPERATOR,   OP_EQUAL);
  ADD_TOKEN("+=",         TOK_OPERATOR,   OP_ASSIGN_PLUS);
  ADD_TOKEN("-=",         TOK_OPERATOR,   OP_ASSIGN_MINUS);
  ADD_TOKEN("*=",         TOK_OPERATOR,   OP_ASSIGN_MULTIPLY);
  ADD_TOKEN("/=",         TOK_OPERATOR,   OP_ASSIGN_DIVIDE);
  ADD_TOKEN("%=",         TOK_OPERATOR,   OP_ASSIGN_MODULO);
  ADD_TOKEN("&=",         TOK_OPERATOR,   OP_ASSIGN_AND);
  ADD_TOKEN("|=",         TOK_OPERATOR,   OP_ASSIGN_OR);
  ADD_TOKEN("^=",         TOK_OPERATOR,   OP_ASSIGN_XOR);
  ADD_TOKEN("++",         TOK_OPERATOR,   OP_INCREMENT);
  ADD_TOKEN("--",         TOK_OPERATOR,   OP_DECREMENT);
  ADD_TOKEN("&&",         TOK_OPERATOR,   OP_LOGICAL_AND);
  ADD_TOKEN("||",         TOK_OPERATOR,   OP_LOGICAL_OR);
  ADD_TOKEN("->",         TOK_OPERATOR,   OP_ARROW);
  ADD_TOKEN(">=",         TOK_OPERATOR,   OP_LOGICAL_GTE);
  ADD_TOKEN("<=",         TOK_OPERATOR,   OP_LOGICAL_LTE);
  ADD_TOKEN("!",          TOK_OPERATOR,   OP_LOGICAL_NOT);
  ADD_TOKEN("~",          TOK_OPERATOR,   OP_BITWISE_NOT);
  ADD_TOKEN(";",          TOK_OPERATOR,   OP_SEMICOLON);
  ADD_TOKEN(":",          TOK_OPERATOR,   OP_COLON);
  ADD_TOKEN("?",          TOK_OPERATOR,   OP_QUESTIONMARK);
  ADD_TOKEN(",",          TOK_OPERATOR,   OP_COMMA);
  ADD_TOKEN(".",          TOK_OPERATOR,   OP_DOT);
  ADD_TOKEN("+",          TOK_OPERATOR,   OP_PLUS);
  ADD_TOKEN("-",          TOK_OPERATOR,   OP_MINUS);
  ADD_TOKEN("*",          TOK_OPERATOR,   OP_MULTIPLY);
  ADD_TOKEN("/",          TOK_OPERATOR,   OP_DIVIDE);
  ADD_TOKEN(">",          TOK_OPERATOR,   OP_LOGICAL_GT);
  ADD_TOKEN("<",          TOK_OPERATOR,   OP_LOGICAL_LT);
  ADD_TOKEN("&",          TOK_OPERATOR,   OP_BITWISE_AND);
  ADD_TOKEN("|",          TOK_OPERATOR,   OP_BITWISE_OR);
  ADD_TOKEN("^",          TOK_OPERATOR,   OP_BITWISE_XOR);
  ADD_TOKEN("%",          TOK_OPERATOR,   OP_MODULO);
  ADD_TOKEN("=",          TOK_OPERATOR,   OP_ASSIGN);
  ADD_TOKEN("(",          TOK_OPERATOR,   OP_LPAREN);
  ADD_TOKEN(")",          TOK_OPERATOR,   OP_RPAREN);
  ADD_TOKEN("{",          TOK_OPERATOR,   OP_LBRACE);
  ADD_TOKEN("}",          TOK_OPERATOR,   OP_RBRACE);
  ADD_TOKEN("[",          TOK_OPERATOR,   OP_LBRACKET);
  ADD_TOKEN("]",          TOK_OPERATOR,   OP_RBRACKET);

  /* Setup sentinel-index for first keyword-token.
     This is used to stop greedy keyword-matching inside identifiers. */
  idx_first_tok_keyword = OP_RBRACKET + 1;

  /* Some C99 Keywords */
  ADD_TOKEN("int8_t",     TOK_KEYWORD,    KW_INT);
  ADD_TOKEN("int16_t",    TOK_KEYWORD,    KW_INT);
  ADD_TOKEN("int32_t",    TOK_KEYWORD,    KW_INT);
  ADD_TOKEN("int64_t",    TOK_KEYWORD,    KW_INT);
  ADD_TOKEN("uint8_t",    TOK_KEYWORD,    KW_INT);
  ADD_TOKEN("uint16_t",   TOK_KEYWORD,    KW_INT);
  ADD_TOKEN("uint32_t",   TOK_KEYWORD,    KW_INT);
  ADD_TOKEN("uint64_t",   TOK_KEYWORD,    KW_INT);

  /* The 32 reserved keywords of C: */
  ADD_TOKEN("auto",       TOK_KEYWORD,    KW_AUTO);
  ADD_TOKEN("break",      TOK_KEYWORD,    KW_BREAK);
  ADD_TOKEN("case",       TOK_KEYWORD,    KW_CASE);
  ADD_TOKEN("char",       TOK_KEYWORD,    KW_CHAR);
  ADD_TOKEN("const",      TOK_KEYWORD,    KW_CONST);
  ADD_TOKEN("continue",   TOK_KEYWORD,    KW_CONTINUE);
  ADD_TOKEN("default",    TOK_KEYWORD,    KW_DEFAULT);
  ADD_TOKEN("do",         TOK_KEYWORD,    KW_DO);
  ADD_TOKEN("double",     TOK_KEYWORD,    KW_DOUBLE);
  ADD_TOKEN("else",       TOK_KEYWORD,    KW_ELSE);
  ADD_TOKEN("enum",       TOK_KEYWORD,    KW_ENUM);
  ADD_TOKEN("extern",     TOK_KEYWORD,    KW_EXTERN);
  ADD_TOKEN("float",      TOK_KEYWORD,    KW_FLOAT);
  ADD_TOKEN("for",        TOK_KEYWORD,    KW_FOR);
  ADD_TOKEN("goto",       TOK_KEYWORD,    KW_GOTO);
  ADD_TOKEN("if",         TOK_KEYWORD,    KW_IF);
  ADD_TOKEN("int",        TOK_KEYWORD,    KW_INT);
  ADD_TOKEN("long",       TOK_KEYWORD,    KW_LONG);
  ADD_TOKEN("register",   TOK_KEYWORD,    KW_REGISTER);
  ADD_TOKEN("return",     TOK_KEYWORD,    KW_RETURN);
  ADD_TOKEN("short",      TOK_KEYWORD,    KW_SHORT);
  ADD_TOKEN("signed",     TOK_KEYWORD,    KW_SIGNED);
  ADD_TOKEN("sizeof",     TOK_KEYWORD,    KW_SIZEOF);
  ADD_TOKEN("static",     TOK_KEYWORD,    KW_STATIC);
  ADD_TOKEN("struct",     TOK_KEYWORD,    KW_STRUCT);
  ADD_TOKEN("switch",     TOK_KEYWORD,    KW_SWITCH);
  ADD_TOKEN("typedef",    TOK_KEYWORD,    KW_TYPEDEF);
  ADD_TOKEN("union",      TOK_KEYWORD,    KW_UNION);
  ADD_TOKEN("unsigned",   TOK_KEYWORD,    KW_UNSIGNED);
  ADD_TOKEN("void",       TOK_KEYWORD,    KW_VOID);
  ADD_TOKEN("volatile",   TOK_KEYWORD,    KW_VOLATILE);
  ADD_TOKEN("while",      TOK_KEYWORD,    KW_WHILE);
}





/* assertion function that prints out line and char number before exiting. */
static void _expect(struct lexer* l, int p, int line)
{
  if (!p)
  {
    fprintf(stderr, "lexer.c line %d ", line);
    fprintf(stderr, "ERROR @ line %u:%u.\n", l->cur_lineno, l->cur_byteno);
  }
  assert(p);
}


static void print_token(struct lexer* l, struct token* t)
{
  expect(l, t != 0);

  const char* tok_knds[] =
  {
    "TOK_EOF",
    "TOK_CONST",
    "TOK_OPERATOR",
    "TOK_KEYWORD",
    "TOK_IDENTIFIER",
  };

  //printf("'%s' @ %u:%u [%d/%d] \n", t->symbol, t->lineno, t->byteno, t->tokknd, t->toktyp);
  fprintf(stdout, "%-15s : '%s' @ %u:%u \n", tok_knds[t->tokknd], t->symbol, t->lineno, t->byteno);
}


/* emit a token built from characters accumulated in token_buffer. */
static void emit(struct lexer* l, struct token* next_tok, int token_kind, int token_type)
{
  expect(l, l->token_length > 0);
  l->token_buffer[l->token_length] = 0;
  next_tok->symbol  = str_copy(l->token_buffer);
  next_tok->symlen  = l->token_length;
  next_tok->tokknd  = token_kind;
  next_tok->toktyp  = token_type;
  next_tok->lineno  = l->cur_lineno;
  next_tok->byteno  = l->cur_byteno;
  next_tok->foffset = (l->buffer - l->buffer_original);

  if (next_tok->tokknd == TOK_IDENTIFIER) /* Did we match a keyword? */
  {
    uint32_t i;
    for (i = idx_first_tok_keyword; i < l->nkeywords; ++i)
    {
      if (    (l->lexemes[i].tokknd == TOK_KEYWORD)
           && (l->lexemes[i].symlen == l->token_length)
           && (strncmp(l->token_buffer, l->lexemes[i].symbol, l->token_length) == 0))
      {
        next_tok->tokknd = l->lexemes[i].tokknd;
        next_tok->toktyp = l->lexemes[i].toktyp;
        break;
      }
    }
  }

  l->token_length = 0;
}

/* skip to next token while handling line-number and column/byte-number. */
static void next(struct lexer* l)
{
  expect(l, l->buffer != 0);

  if (l->buffer[0] == '\n')
  {
    l->cur_lineno += 1;
    l->cur_byteno = 0;
  }
  l->cur_byteno += 1;
  l->buffer += 1;
}


/* add the current character to l->token_buffer and advance to next char in input stream. */
static void consume(struct lexer* l)
{
  l->token_buffer[l->token_length++] = l->buffer[0];
  expect(l, l->token_length < sizeof(l->token_buffer));
  next(l);
}


static void skip_comments(struct lexer* l)
{
  /* Handle comments: */
  if (    (l->buffer[0] == '/')           // C style comment: /* ... */
       && (l->buffer[1] == '*'))
  {
    next(l);   /* skip </> */
    next(l);   /* skip <*> */
    while (    (l->buffer[0] != 0)        /* Avoid reading garbage */
            && (    (l->buffer[0] != '*')
                 || (l->buffer[1] != '/')))
    {
      next(l); /* body of comment: ignore it */
    }
    next(l);   /* skip <*> */
    next(l);   /* skip </> */
  }
  else if (    (l->buffer[0] == '/')      /* C++ style comment: // .... */
            && (l->buffer[1] == '/'))
  {
    while (    (l->buffer[0] != 0)        /* Avoid reading garbage */
            && (l->buffer[0] != '\n'))    /* Consume chars until line ends */
    {
      next(l);
    }
  }
}



