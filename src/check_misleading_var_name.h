#ifndef __CHECK_MISLEADING_VAR_NAME_H__
#define __CHECK_MISLEADING_VAR_NAME_H__


#include "lexer.h"
#include "source.h"



void check_misleading_var_name_init(void);
void check_misleading_var_name_new_token(struct source_file* s, struct token* toks, int tok_idx);



#endif /* __CHECK_MISLEADING_VAR_NAME_H__ */


