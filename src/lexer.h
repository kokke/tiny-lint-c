/*

  A simple lexer for C-like languages
  ===================================

    Matches:
    --------
      - single- and multi-line comments
      - string literals
      - char literals
      - integer numerals
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

    - when times comes, to implement support for '#line XX "file_yy.c" ... ' etc.,
      just make the name point into the source code and use the file_path_length
 
    

*/

#ifndef __LEXER_H__
#define __LEXER_H__

#include "token.h"
#include <stdint.h>


#define MAXNTOKENS     1024 /* max number of keywords/lexemes to support - set high enough to accomodate typedef'd types etc. */
#define MAXTOKENLEN   65536 /* max supported token_length - this is the maximum supported token (and string) length. */




/* Lexer context object */
struct lexer 
{
  struct token lexemes[MAXNTOKENS];   /* Set of valid C tokens: operators + keywords (excluding trigraphs). */
  uint32_t nkeywords;                 /* Number of keywords in lexemes[]. */
  char*    buffer;                    /* Pointer to char buffer where tokens are read from (src file). */
  char*    buffer_original;           /* Pointer to start of buffer - 'buffer' points to next lex-point. */
  char     token_buffer[MAXTOKENLEN]; /* Buffer to accumulate chars to build current token from. */
  uint32_t token_length;              /* Length of current token. */
  uint32_t cur_lineno;                /* Line number in current input source code file. */
  uint32_t cur_byteno;                /* Byte/column number in current line. */
  int      continue_on_error;      
};



void lexer_init(struct lexer* l);
void lexer_free(struct lexer* l);
void lexer_reset_state(struct lexer* l);
void lexer_setup_alphabet(struct lexer* l);
void lexer_set_char_buf(struct lexer* l, char* char_buf);
void lexer_add_token(struct lexer* l, char* token_symbol, int token_strlen, int token_kind, int token_type);

struct token lexer_next_token(struct lexer* l);

#endif /* __LEXER_H__ */

