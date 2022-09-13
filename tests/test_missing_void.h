/*

Find cases of 'f()' instead of 'f(void)' in function declarations / definitions

*/

#include "non_existing_file.h"

#define STUPID_MACRO 5

extern int x;


void function1();      /* HIT */
void function2(void);


void function3 ()      /* HIT */
{
  return 42;
}


static const unsigned int const* function4(void);
static const unsigned int const* function5(); /* HIT */


