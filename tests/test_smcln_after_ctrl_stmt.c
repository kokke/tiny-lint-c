

int main(void)
{
  int x = rand();
  if (x > 42); /* HIT */
  {
    printf("then do something else!");
  }

  int i;
  for (i = 0; i < x; ++i); { /* HIT */
    printf("then do something else!");
  }

  i = 0;
  do
  {
    i +=1;
    int j;
    for (j = 0; j < 5; ++j)
    {
      if (    (i > j)
           && ((i - j) > 1))
      {
        printf(".....");
      }
      do { printf("."); } while(0);
    }
  } while (i < 10);

  while (x > 42); { /* HIT */
    printf("then do something else!");
  }


  return 0;
}


