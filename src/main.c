/*

Static analysis at the lexical abstraction level

*/


#include <assert.h>              /* for assert            */
#include <stdio.h>               /* for printf + fgetc    */
#include "lexer.h"
#include "analysis.h"




/* Main driver: */
int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    fprintf(stderr, "\nError: No file-list given as input\n\nUsage: %s <input-file>\n\n", argv[0]);
    return 1;
  }

  FILE* f = fopen(argv[1], "rb");
  if (f != 0)
  {
    struct lexer l;

    lexer_init(&l);
    lexer_setup_alphabet(&l);

    l.continue_on_error = 1;

    char file_path[1024];
    int  file_len = 0;
    int  c;
    while ((c = fgetc(f)) != EOF)
    {
      /* begin scanning a new file from 'file_path' */
      if ((char)c == '\n')
      {
        file_path[file_len] = 0; /* terminate string */
        analysis_check_file(&l, file_path);
        file_len = 0;
      }
      else
      {
        file_path[file_len++] = (char)c;
      }
    }
    fclose(f);
    lexer_free(&l);

    return 0;
  }

  return 1;
}





