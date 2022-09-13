#include "source.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static size_t _fsize(const char* file)
{
  size_t fsize = 0;
  FILE * f = fopen(file, "r");
  if (f != 0)
  {
    fseek(f, 0, SEEK_END);
    size_t len = ftell(f);
    fclose(f);
    fsize = len;
  }
  return fsize;
}

int src_init(struct source_file* src, const char* file_path)
{
  int success = 0;
  if (    (src != 0)
       && (file_path != 0))
  {
    src->file_path_len = strlen(file_path) + 1; /* termination byte! */
    src->file_path = malloc(src->file_path_len);
    assert(src->file_path != 0);
    memcpy(src->file_path, file_path, src->file_path_len);
  //src->file_path = strdup(file_path);
    
    src->file_content = 0;
    src->nlines = 0;
    src->file_size = _fsize(file_path);
    if (src->file_size > 0)
    {
      src->file_content = malloc(src->file_size + 10);
      assert(src->file_content != 0);
      memset(src->file_content, 0, src->file_size + 10);
    }
    success = 1;
  }
  return success;
}


int src_read_content(struct source_file* src)
{
  int nbytes_read = 0;
  if (    (src != 0)
       && (src->file_size > 0)
       && (src->file_path !=0))
  {
    FILE* fp = fopen(src->file_path, "rb");
    if (fp != 0)
    {
      int c;
      uint32_t nbytes = 0;
      /* Read file, null-terminate buffer and close file again */
      do
      {
        c = fgetc(fp);
        src->file_content[nbytes++] = c;
        src->nlines += (c == '\n');
      }
      while (c != EOF);       /* Read until EOF */
      fclose(fp);             /* Close file again */
      src->file_content[nbytes - 1] = 0; /* Overwrite EOF with null-termination */
        
      nbytes_read = (int)nbytes;
    }
  }
  return nbytes_read;
}

int src_free(struct source_file* src)
{
  /* Macro to save a little typing */
  #define FREE(x)   if ((x) != 0) { free((x)); x = 0; }

  int success = 0;
  if (src != 0)
  {
    FREE(src->file_path);
    if (src->file_size > 0)
    {
      FREE(src->file_content);
    }
    success = 1;
  }
  return success;
}




