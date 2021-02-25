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

static int function2(void)
{
  if (rand() == 0) return 0;
  if (rand() == 1) return 1;
  if (rand() == 2) return 2;
  if (rand() == 3) return 3; /* HIT */
  if (rand() == 4) return 4; /* HIT */
  if (rand() == 5) return 5; /* HIT */
  if (rand() == 6) return 6; /* HIT */
}

static int function3(int x)
{
  if (x & 1) return 0;
  else return 1;
}




int main(void)
{
  int x = function1(rand());
  function2();
  int y = function3(x);

  return 0;
}


