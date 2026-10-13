#include "check_smcln_after_ctrl_stmt.h"
#include "check_common.h"
#include <stdio.h>


enum state
{
  S_IDLE            =  0, /* looking for a control statement        */
  S_DO_BODY         = -1, /* inside do { ... }, awaiting while ;     */
  S_AFTER_KEYWORD   =  1, /* saw if/while/for, expecting '('         */
  S_AFTER_PAREN     =  2, /* saw matching ')', expecting ';' or '{'  */
  S_AFTER_SEMICOLON =  3, /* saw ')' then ';', warn if next is '{'   */
};

static struct nesting n;
static int paren_lvl_s1 = 0;
static int do_brace_lvl = 0;
static enum state state = S_IDLE;
static enum ctrl_kind kind = CTRL_IF;


/* reset balance counter when loading new file */
void check_smcln_after_ctrl_stmt_init(void)
{
  nesting_reset(&n);
  paren_lvl_s1 = 0;
  do_brace_lvl = 0;
  state = S_IDLE;
  kind = CTRL_IF;
}

static void _reset(void)
{
  state = S_IDLE;
}

void check_smcln_after_ctrl_stmt_new_token(struct source_file* s, struct token* toks, int tok_idx)
{
  int i = tok_idx;

  if (    (state == S_DO_BODY)
       && (toks[i].toktyp == OP_SEMICOLON)
       && ((do_brace_lvl - n.brace_lvl) == 0))
  {
    state = S_IDLE;
  }
  else if (state == S_IDLE)
  {
    if (    (toks[i - 1].toktyp == KW_DO)
         && (toks[  i  ].toktyp == OP_LBRACE))
    {
      state = S_DO_BODY;
      do_brace_lvl = n.brace_lvl;
    }
    else if (    (toks[i].toktyp == KW_IF)
              || (toks[i].toktyp == KW_FOR)
              || (toks[i].toktyp == KW_WHILE))
    {
      state = S_AFTER_KEYWORD;
      paren_lvl_s1 = n.paren_lvl;
      kind = ctrl_kind_from_symbol(&toks[i]);
    }
  }
  else if (state == S_AFTER_KEYWORD)
  {
    if (    (toks[i].toktyp == OP_RPAREN)
         && ((n.paren_lvl - paren_lvl_s1) == 1))
    {
      state = S_AFTER_PAREN;
    }
    else if (toks[i].toktyp == OP_SEMICOLON && kind != CTRL_FOR)
    {
      _reset();
    }
  }
  else if (state == S_AFTER_PAREN)
  {
    if (toks[i].toktyp == OP_SEMICOLON)
    {
      state = S_AFTER_SEMICOLON;
    }
    else
    {
      _reset();
    }
  }
  else if (state == S_AFTER_SEMICOLON)
  {
    if (toks[i].toktyp == OP_LBRACE)
    {
      fprintf(stdout, "[%s:%d] (warning) Suspicious semicolon after %s-stmt.\n", s->file_path, toks[i - 1].lineno, ctrl_kind_names[kind] );
    }
    _reset();
  }

  nesting_update(&n, &toks[i]);
}




