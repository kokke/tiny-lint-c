#include "hej_mor.h"

#define LOLOLO 5

extern int x;


void function1(uint32_t u8varName);   /* HIT: u8varName is of type uint32_t */
void function2(void);


void function3 (int8_t   u32var)      /* HIT: u32var is of type int8_t */
{
  return 42;
}

void function4(uint16_t u8varName);   /* HIT: u8varName is of type uint16_t  */
void function5(uint64_t u8varName);   /* HIT: u8varName is of type uint64_t  */
void function6(int16_t u8varName);    /* HIT: u8varName is of type int16_t   */
void function7(int64_t u8varName);    /* HIT: u8varName is of type int64_t   */
void function8(int16_t s16varName);   /* s16varName is of type int64_t  */
void function9(int64_t s64varName);   /* s64varName is of type int64_t  */
void function10(uint16_t s16varName); /* HIT: s16varName is of type uint16_t */
void function11(uint64_t s64varName); /* HIT: s64varName is of type uint64_t */



