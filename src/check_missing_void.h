#ifndef __CHECK_MISSING_VOID_H__
#define __CHECK_MISSING_VOID_H__


#include "lexer.h"
#include "source.h"

void check_missing_void_init(void);
void check_missing_void_new_token(struct source_file* s, struct token* toks, int tok_idx);



#endif /* __CHECK_MISSING_VOID_H__ */


