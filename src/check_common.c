#include "check_common.h"


const char* const ctrl_kind_names[3] = { "if", "while", "for" };


void nesting_update(struct nesting* n, const struct token* tok)
{
       if (tok->toktyp == OP_LBRACE)   { n->brace_lvl   += 1; }
  else if (tok->toktyp == OP_RBRACE)   { n->brace_lvl   -= 1; }
  else if (tok->toktyp == OP_LPAREN)   { n->paren_lvl   += 1; }
  else if (tok->toktyp == OP_RPAREN)   { n->paren_lvl   -= 1; }
  else if (tok->toktyp == OP_LBRACKET) { n->bracket_lvl += 1; }
  else if (tok->toktyp == OP_RBRACKET) { n->bracket_lvl -= 1; }

  if (n->paren_lvl < 0)   { n->paren_lvl   = 0; }
  if (n->brace_lvl < 0)   { n->brace_lvl   = 0; }
  if (n->bracket_lvl < 0) { n->bracket_lvl = 0; }
}

void nesting_reset(struct nesting* n)
{
  n->paren_lvl   = 0;
  n->brace_lvl   = 0;
  n->bracket_lvl = 0;
}

enum ctrl_kind ctrl_kind_from_symbol(const struct token* tok)
{
  switch (tok->symbol[0])
  {
    case 'w': return CTRL_WHILE;
    case 'f': return CTRL_FOR;
    default:  return CTRL_IF;
  }
}
