#ifndef __CHECK_COMMON_H__
#define __CHECK_COMMON_H__



#include "source.h"


/* Running '(', ')' and '{', '}' balance, shared by the checks. */
struct nesting
{
  int paren_lvl;
  int brace_lvl;
  int bracket_lvl;
};

/* Update paren/brace balance for one token, clamped at 0. */
void nesting_update(struct nesting* n, const struct token* tok);

/* Reset paren/brace/bracket balance to 0. */
void nesting_reset(struct nesting* n);


/* Control-statement kind; values double as str index below. */
enum ctrl_kind
{
  CTRL_IF    = 0,
  CTRL_WHILE = 1,
  CTRL_FOR   = 2,
};

/* Classify an if/while/for keyword token by its first character. */
enum ctrl_kind ctrl_kind_from_symbol(const struct token* tok);

/* Names indexed by enum ctrl_kind: {"if", "while", "for"}. */
extern const char* const ctrl_kind_names[3];



#endif /* __CHECK_COMMON_H__ */
