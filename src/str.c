#include "str.h"
#include <assert.h>
#include <string.h>


#define STR_BUF_SZ          (4 * 1024 * 1024) /* string-buffer size */

static char str_buffer[STR_BUF_SZ];           /* string-buffer */
static int  str_idx;                          /* index -- how many bytes are in use */



void str_init(void)
{
  if (str_idx > 0)
  {
    memset(str_buffer, 0, str_idx);
  }
  str_idx = 0;
}


char* str_copy(const char* string)
{
  char* ret = &str_buffer[str_idx];
  int i;
  for (i = 0; i < (STR_BUF_SZ - str_idx); ++i)
  {
    str_buffer[str_idx + i] = string[i];
    if (string[i] == 0)
    {
      break;
    }
  }
  str_idx += i + 1;

  assert(str_idx < STR_BUF_SZ);

  return ret;
}





