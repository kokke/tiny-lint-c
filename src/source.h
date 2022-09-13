#ifndef __SOURCE_H__
#define __SOURCE_H__


#include "lexer.h"
#include <stdint.h> /* for intX_t            */

/*
  Structure associating source file with:
   - Array of tokens lexed from file
   - Path-to-file and file contents
   - File size in bytes and line count
*/
struct source_file
{
  struct token* tokens;    /* token array.                   */
  uint32_t ntokens;        /* length of token-array.         */
  char*    file_path;      /* path to file.                  */
  uint32_t file_path_len;  /* length of file_path in bytes.  */
  char*    file_content;   /* raw file contents.             */
  uint32_t file_size;      /* length of raw file contents.   */
  uint32_t nlines;         /* number of lines in file.       */
};



int src_init(struct source_file* src, const char* file_path);
int src_read_content(struct source_file* src);
int src_free(struct source_file* src);



#endif /* __SOURCE_H__ */


