/*

Static analysis at the lexical abstraction level


*/

#include <assert.h>              /* for assert            */
#include <stdio.h>               /* for printf + fgetc    */
#include "lexer.h"
#include "source.h"
#include "str.h"
#include "check_assign_in_ctrl_stmt.h"       /* check for assignments in expressions affecting control flow */
#include "check_missing_void.h"              /* check fundecls for missing (void), e.g. f() vs f(void) <-- correct */
#include "check_misleading_var_name.h"       /* check if variable names are misleading, e.g. if u32var is of type int8_t */
#include "check_smcln_after_ctrl_stmt.h"     /* check for suspicious semicolons, e.g. 'if (X); { <always-on> }' etc. */
#include "check_return_count_in_functions.h" /* check how many return-statements are in a function, warn on too many */


/* max size of token-buffer for each file */
#define MAXTOKENBUFSIZE (1024*1024)/1



static void analysis_new_file(void);
static void analysis_new_token(void);



static struct token toks[MAXTOKENBUFSIZE];
static struct source_file s;
static int ntokens;



void analysis_check_file(struct lexer* l, const char* src_file)
{
  /* Determine input source and test if it can be found and read.
     If src_file can be found, src_init() dynamically allocates memory. */
  int src_init_success = src_init(&s, src_file);
  assert(src_init_success == 1);

  /* Reclaim string-buffer memory  */
  str_init();

  /* Read file, null-terminate buffer and close file again */
  if (src_read_content(&s) > 0)
  {
    /* Initialize lexer and pass source file */
    lexer_set_char_buf(l, s.file_content);

    /* (Re-)Initialize checkers */
    analysis_new_file();

    /* ====================================== */
    /* Tokenize file and build token-stream:  */
    /* ====================================== */

    /* Re-use token-buffer for the new file */
    ntokens = 0;

    /* Turn characters into tokens / lexemes: */
    while (l->buffer[0] != 0)
    {
      struct token t = lexer_next_token(l);
      if (ntokens < MAXTOKENBUFSIZE)
      {
        toks[ntokens] = t;
      }
      else if (ntokens == MAXTOKENBUFSIZE)
      {
        fprintf(stderr, "WARNING: max token buffer size exceeded, increase %s\n", "MAXTOKENBUFSIZE");
      }

      ntokens += 1;

      if (t.tokknd == TOK_EOF)
      {
        break;
      }

      analysis_new_token();
    }
  }
  /* Clean up memory dynamically allocated by src_init(). */
  src_free(&s);
}





static void analysis_new_file(void)
{
  /* (Re-)Initialize checkers */
  check_assign_in_ctrl_stmt_init();
  check_missing_void_init();
  check_misleading_var_name_init();
  check_smcln_after_ctrl_stmt_init();
  check_return_count_in_functions_init();
}


static void analysis_new_token(void)
{
  /* Pass the new token to each checker in turn */
  int tok_idx = (ntokens - 1);

  /* Disabled checks:
     None at the moment... */
  
  check_assign_in_ctrl_stmt_new_token(&s, toks, tok_idx);  
  check_missing_void_new_token(&s, toks, tok_idx);
  check_misleading_var_name_new_token(&s, toks, tok_idx);
  check_smcln_after_ctrl_stmt_new_token(&s, toks, tok_idx);
  check_return_count_in_functions_new_token(&s, toks, tok_idx);
}


static void analysis_end_of_file(void)
{
  /* De-Initialize checkers */
  /*
  check_assign_in_ctrl_stmt_eof();
  check_missing_void_eof();
  check_misleading_var_name_eof();
  check_smcln_after_ctrl_stmt_eof();
  */
}



