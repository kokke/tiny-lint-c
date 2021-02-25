static int function1(int x)
{
  if (x == 1) return 1;
  if (x == 2) return 2;
  if (x == 3) return 3;
  if (x == 4) return 4; /* HIT */
  if (x == 5) return 5; /* HIT */
  if (x == 6) return 6; /* HIT */
  if (x == 7) return 7; /* HIT */
}

static int function2(void)
{
  if (rand() == 1) return 1;
  if (rand() == 2) return 2;
  if (rand() == 3) return 3;
  if (rand() == 4) return 4; /* HIT */
  if (rand() == 5) return 5; /* HIT */
  if (rand() == 6) return 6; /* HIT */
}

static int function3(int x)
{
  if (x & 1) return 0;
  else return 1;
}


static int function4(void)
{
  if (rand() == 1)  return 1;
  if (rand() == 2)  return 2;
  if (rand() == 3)  return 3;
  if (rand() == 4)  return 4;  /* HIT */
  if (rand() == 5)  return 5;  /* HIT */
  if (rand() == 6)  return 6;  /* HIT */
  if (rand() == 7)  return 7;  /* HIT */
  if (rand() == 8)  return 8;  /* HIT */
  if (rand() == 9)  return 9;  /* HIT */
  if (rand() == 10) return 10; /* HIT */
  if (rand() == 11) return 11; /* HIT */
  if (rand() == 12) return 12; /* HIT */
  if (rand() == 13) return 13; /* HIT */
  if (rand() == 14) return 14; /* HIT */
  if (rand() == 15) return 15; /* HIT */
  if (rand() == 16) return 16; /* HIT */
  if (rand() == 17) return 17; /* HIT */
  if (rand() == 18) return 18; /* HIT */
  if (rand() == 19) return 19; /* HIT */
  if (rand() == 20) return 20; /* HIT */
  if (rand() == 21) return 21; /* HIT */
  if (rand() == 22) return 22; /* HIT */
  if (rand() == 23) return 23; /* HIT */
  if (rand() == 24) return 25; /* HIT */
}


int main(void)
{
  int x = function1(rand());
  function2();
  int y = function3(x);

  return 0;
}


