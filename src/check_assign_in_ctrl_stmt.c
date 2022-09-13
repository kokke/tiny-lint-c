#include "check_assign_in_ctrl_stmt.h"
#include <stdio.h>
#include <string.h> /* strncmp */


static int paren_lvl = 0; /* balance of '(', ')' */
static int paren_lvl_s1 = 0; /* balance of '(', ')' */
static int paren_lvl_s1_max = 0;
static int state = 0;
static int if_while_for = 0;
static int nsemicolons_s1 = 0;
static int ncommas_s1 = 0;
static int ncommas_defect = 0;
static int found_defect = 0;



/* reset balance counter when loading new file */
void check_assign_in_ctrl_stmt_init(void)
{
  paren_lvl = 0;
  state = 0;
  if_while_for = 0;
  paren_lvl_s1 = 0;
  ncommas_s1 = 0;
  ncommas_defect = 0;
  found_defect = 0;
}

void check_assign_in_ctrl_stmt_new_token(struct source_file* s, struct token* toks, int tok_idx)
{
  int i = tok_idx;

  if (    (    (strncmp("if", toks[i].symbol, 2)    == 0)
            && (toks[i].symlen == 2)        )
       || (    (strncmp("while", toks[i].symbol, 5) == 0)
            && (toks[i].symlen == 5)        )
       || (    (strncmp("for", toks[i].symbol, 3)   == 0)
            && (toks[i].symlen == 3)        ))
  {
    /*
    printf("toks[i].toktyp == %d\n", toks[i].toktyp);
    printf("toks[i].symbol == '%s'\n", toks[i].symbol);
    */
    found_defect = 0;
    state = 1;
    nsemicolons_s1 = 0;
    paren_lvl_s1 = paren_lvl;
    switch (toks[i].symbol[0])
    {
      case 'i': if_while_for = 0; break;
      case 'w': if_while_for = 1; break;
      case 'f': if_while_for = 2; break;
    }
  }
  else if (state == 1)
  {
    if (toks[i].symbol[0] == ';')
    {
      nsemicolons_s1 += 1;
    }
    else if (toks[i].symbol[0] == ',')
    {
      ncommas_s1 += 1;
    }

    if (    (toks[i].toktyp == OP_RPAREN)
         && ((paren_lvl - paren_lvl_s1) == 1))
    {
      state = 0;
      if (found_defect && (ncommas_s1 == ncommas_defect))
      {
        const char* str_ifw[] = { "if", "while", "for" };
        if (if_while_for != 2 || ((nsemicolons_s1 == 3) || (nsemicolons_s1 == 1))) /* for-loop? then check middle expression Y in 'FOR ( X ; Y ; Z )' */
        {
          fprintf(stdout, "[%s:%d] (warning) Assignment in expression controlling program flow (%s-stmt).\n", s->file_path, toks[i - 1].lineno, str_ifw[if_while_for]);
        }
      }
    }
    else if (    ((paren_lvl - paren_lvl_s1) == 1)
              && (    (toks[i].toktyp == OP_ASSIGN)
                   || (toks[i].toktyp == OP_ASSIGN_LSH)
                   || (toks[i].toktyp == OP_ASSIGN_RSH)
                   || (toks[i].toktyp == OP_ASSIGN_PLUS)
                   || (toks[i].toktyp == OP_ASSIGN_MINUS)
                   || (toks[i].toktyp == OP_ASSIGN_MULTIPLY)
                   || (toks[i].toktyp == OP_ASSIGN_DIVIDE)
                   || (toks[i].toktyp == OP_ASSIGN_AND)
                   || (toks[i].toktyp == OP_ASSIGN_OR)
                   || (toks[i].toktyp == OP_ASSIGN_XOR))     )
    {
      ncommas_defect = ncommas_s1;
      found_defect = 1;
    }
  }

       if (toks[i].toktyp == OP_LPAREN) { paren_lvl += 1; }
  else if (toks[i].toktyp == OP_RPAREN) { paren_lvl -= 1; }

  if (paren_lvl < 0) { paren_lvl = 0; }

}

