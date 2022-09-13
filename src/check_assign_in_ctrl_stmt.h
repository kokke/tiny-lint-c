#ifndef __CHECK_ASSIGN_IN_CTRL_STMT_H__
#define __CHECK_ASSIGN_IN_CTRL_STMT_H__



#include "lexer.h"
#include "source.h"

void check_assign_in_ctrl_stmt_init(void);
void check_assign_in_ctrl_stmt_new_token(struct source_file* s, struct token* toks, int tok_idx);



#endif /* __CHECK_ASSIGN_IN_CTRL_STMT_H__ */


