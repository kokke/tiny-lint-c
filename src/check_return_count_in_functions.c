#include "check_return_count_in_functions.h"
#include <stdio.h>
#include <string.h> /* strncmp */


static int paren_lvl = 0;    /* balance of '(', ')' */
static int brace_lvl = 0;    /* balance of '{', '}' */
static int state = 0;
static int state_last_warn = 0;


/* reset balance counter when loading new file */
void check_return_count_in_functions_init(void)
{
  paren_lvl = 0;
  brace_lvl = 0;
  state = 0;
}

void check_return_count_in_functions_new_token(struct source_file* s, struct token* toks, int tok_idx)
{
  int i = tok_idx;

  if (brace_lvl >= 1)
  {
    if (toks[i].toktyp == KW_RETURN)
    {
      state += 1;
    }
  }
  else if (brace_lvl == 0)
  {
    state = 0;
  }

  if (state > MAX_RETURNS_PR_FUNC)
  {
    /* one warning pr. violation is sufficient */
    if (state != state_last_warn)
    {
      state_last_warn = state;
      fprintf(stdout, "[%s:%d] (style) %d return statements in a single function.\n", s->file_path, toks[i - 1].lineno, state);
    }
  }

       if (toks[i].toktyp == OP_LBRACE) { brace_lvl += 1; }
  else if (toks[i].toktyp == OP_RBRACE) { brace_lvl -= 1; }
  else if (toks[i].toktyp == OP_LPAREN) { paren_lvl += 1; }
  else if (toks[i].toktyp == OP_RPAREN) { paren_lvl -= 1; }

  if (paren_lvl < 0) { paren_lvl = 0; }
  if (brace_lvl < 0) { brace_lvl = 0; }

}

