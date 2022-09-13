#ifndef __CHECK_SMCLN_AFTER_CTRL_STMT_H__
#define __CHECK_SMCLN_AFTER_CTRL_STMT_H__



#include "lexer.h"
#include "source.h"

void check_smcln_after_ctrl_stmt_init(void);
void check_smcln_after_ctrl_stmt_new_token(struct source_file* s, struct token* toks, int tok_idx);



#endif /* __CHECK_SMCLN_AFTER_CTRL_STMT_H__ */


