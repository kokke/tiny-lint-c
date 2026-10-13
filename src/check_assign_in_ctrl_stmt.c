#include "check_assign_in_ctrl_stmt.h"
#include "check_common.h"
#include <stdio.h>
#include <string.h> /* strncmp */


enum state
{
  S_IDLE         = 0, /* outside any control expression */
  S_IN_CTRL_EXPR = 1, /* inside if/while/for ( ... )    */
};

static struct nesting n;
static int paren_lvl_s1 = 0;
static enum state state = S_IDLE;
static enum ctrl_kind kind = CTRL_IF;
static int nsemicolons_s1 = 0;
static int ncommas_s1 = 0;
static int ncommas_defect = 0;
static int found_defect = 0;



/* reset balance counter when loading new file */
void check_assign_in_ctrl_stmt_init(void)
{
  nesting_reset(&n);
  state = S_IDLE;
  kind = CTRL_IF;
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
    state = S_IN_CTRL_EXPR;
    nsemicolons_s1 = 0;
    paren_lvl_s1 = n.paren_lvl;
    kind = ctrl_kind_from_symbol(&toks[i]);
  }
  else if (state == S_IN_CTRL_EXPR)
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
         && ((n.paren_lvl - paren_lvl_s1) == 1))
    {
      state = S_IDLE;
      if (found_defect && (ncommas_s1 == ncommas_defect))
      {
        if (kind != CTRL_FOR || ((nsemicolons_s1 == 3) || (nsemicolons_s1 == 1))) /* for-loop? then check middle expression Y in 'FOR ( X ; Y ; Z )' */
        {
          fprintf(stdout, "[%s:%d] (warning) Assignment in expression controlling program flow (%s-stmt).\n", s->file_path, toks[i - 1].lineno, ctrl_kind_names[kind]);
        }
      }
    }
    else if (    ((n.paren_lvl - paren_lvl_s1) == 1)
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

  nesting_update(&n, &toks[i]);
}

