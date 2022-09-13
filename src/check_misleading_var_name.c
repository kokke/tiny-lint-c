/*

Check if variable names are hinting at the type, and if so, are they hinting at the correct type?
Examples of correct naming:
  uint8_t  u8var;     suggests U8
  int16_t  s16var;    suggests S16
  int32_t  i32var;    suggests S32
  uint32_t var;       no suggestion in var name

Examples of incorrect naming:
  int8_t   u8var      suggests U8, BUT is S8
  uint32_t u16var     suggests U16 but is U32

*/

#include "check_misleading_var_name.h"
#include <stdio.h>  /* fprintf */
#include <string.h> /* strncmp */



static const char* prefixes[]  = { "s8",     "s16",     "s32",     "s64",     "i8",     "i16",     "i32",     "i64",     "u8",      "u16",      "u32",      "u64",      };
static const char* types[]     = { "int8_t", "int16_t", "int32_t", "int64_t", "int8_t", "int16_t", "int32_t", "int64_t", "uint8_t", "uint16_t", "uint32_t", "uint64_t", };
static const int   ltypes[]    = { 6,        7,         7,         7,         6,        7,         7,         7,         6,         7,          7,          7,          };
static const int   lprefixes[] = { 2,        3,         3,         3,         2,        3,         3,         3,         2,         3,          3,          3,          };
static const int   nprefixes   = sizeof(prefixes)/sizeof(*prefixes);
static const int   ntypes      = sizeof(types)/sizeof(*types);


void check_misleading_var_name_init(void)
{
  /* No state to reset */
}



void check_misleading_var_name_new_token(struct source_file* s, struct token* toks, int tok_idx)
{
  /* struct source_file* s is unused */
  (void) s;

  /* we need at least two tokens to run this check */
  if (tok_idx < 1)
  {
    return;
  }

  const char* var_type = toks[tok_idx - 1].symbol;
  const char* var_name = toks[tok_idx].symbol;
  int i, j;

  /* check if previous token was a type and current token has a name that hints at a type: */
  for (i = 0; i < ntypes; ++i)
  {
    /* found a type we have a rule for: */
    if (strncmp(var_type, types[i], ltypes[i]) == 0)
    {
      for (j = 0; j < nprefixes; ++j)
      {
        /* when i == j, we have the correct type prefix -> skip that case */
        if (    (j != i)
             && (strncmp(var_name, prefixes[j], lprefixes[j]) == 0) /* does var_name match a known prefix? */
             && (strncmp(types[i], types[j], ltypes[i]) != 0)       /* signed types can be prefixed I and S, check this here */
             && (    (var_name[lprefixes[j]+1] < '0')               /* next char in var_name must be a digit to match */
                  || (var_name[lprefixes[j]+1] > '9')))
        {
          fprintf(stdout, "[%s:%d] (warning) Variable of type '%s' was named '%s'.\n", s->file_path, toks[tok_idx].lineno, var_type, var_name);
          return; /* maximum one warning pr. token */
        }
      }
    }
  }
}


