static int function1(int x)
{
  if (x == 0) return 0;
  if (x == 1) return 1;
  if (x == 2) return 2;
  if (x == 3) return 3; /* HIT */
  if (x == 4) return 4; /* HIT */
  if (x == 5) return 5; /* HIT */
  if (x == 6) return 6; /* HIT */
  if (x == 7) return 7; /* HIT */
}


int main(void)
{
  return function1(rand());
}


