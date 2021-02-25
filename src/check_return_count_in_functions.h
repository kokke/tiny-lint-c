#ifndef __CHECK_RETURN_IN_FUNC_H__
#define __CHECK_RETURN_IN_FUNC_H__



#include "lexer.h"
#include "source.h"


/* Emit a warning if there are more than 3 return-statements in a function */
#define MAX_RETURNS_PR_FUNC 3


void check_return_count_in_functions_init(void);
void check_return_count_in_functions_new_token(struct source_file* s, struct token* toks, int tok_idx);



#endif /* __CHECK_RETURN_IN_FUNC_H__ */


