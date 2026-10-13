#include "check_missing_void.h"
#include "check_common.h"
#include <stdio.h>


static struct nesting n;


/* reset balance counter when loading new file */
void check_missing_void_init(void)
{
  nesting_reset(&n);
}


void check_missing_void_new_token(struct source_file* s, struct token* toks, int tok_idx)
{
  int i = tok_idx;
    
  /* check for 'function()' instead of 'function(void)' in declarations */
  if (    (i >= 3)
       && (n.brace_lvl == 0)
       && (n.paren_lvl == 1)) /* paren_lvl must b 1, because we act when we see the RPAREN-token - only after seeing the RPAREN, will we decrease paren_lvl */
  {
    if (    (toks[ i ].toktyp == OP_RPAREN)
         && (toks[i-1].toktyp == OP_LPAREN)
         && (toks[i-2].tokknd == TOK_IDENTIFIER)
         && (    (toks[i-3].tokknd == TOK_KEYWORD)
              || (toks[i-3].tokknd == TOK_OPERATOR)
              || (toks[i-3].tokknd == TOK_IDENTIFIER)))
    {
      const char* func_name = toks[i-2].symbol;
      fprintf(stdout, "[%s:%d] (warning) Use '%s(void)' instead of '%s()' in function-declarations.\n", s->file_path, toks[i].lineno, func_name, func_name );
    }
  }

  nesting_update(&n, &toks[i]);
}




