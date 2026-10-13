#include "check_return_count_in_functions.h"
#include "check_common.h"
#include <stdio.h>
#include <string.h> /* strncmp */


static struct nesting n;
static int nreturns = 0;
static int nreturns_last_warn = 0;


/* reset balance counter when loading new file */
void check_return_count_in_functions_init(void)
{
  nesting_reset(&n);
  nreturns = 0;
  nreturns_last_warn = 0;
}

void check_return_count_in_functions_new_token(struct source_file* s, struct token* toks, int tok_idx)
{
  int i = tok_idx;

  if (n.brace_lvl >= 1)
  {
    if (toks[i].toktyp == KW_RETURN)
    {
      nreturns += 1;
    }
  }
  else if (n.brace_lvl == 0)
  {
    nreturns = 0;
  }

  if (nreturns > MAX_RETURNS_PR_FUNC)
  {
    /* one warning pr. violation is sufficient */
    if (nreturns != nreturns_last_warn)
    {
      nreturns_last_warn = nreturns;

      /* prettier names in the output: '3rd return statement in function' etc. */
      const char* endings[] = { "th", "st", "nd", "rd" };
      const char* ending = endings[0];

      if (    (nreturns < 10)
           || (nreturns > 20))
      {
             if (nreturns % 10 == 1) ending = endings[1];
        else if (nreturns % 10 == 2) ending = endings[2];
        else if (nreturns % 10 == 3) ending = endings[3];
      }

      fprintf(stdout, "[%s:%d] (style) %d%s return statement in function.\n", s->file_path, toks[i - 1].lineno, nreturns, ending);
    }
  }

  nesting_update(&n, &toks[i]);
}

