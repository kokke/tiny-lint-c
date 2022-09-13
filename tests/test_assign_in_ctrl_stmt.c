void dummy(void)
{
  int x = 42;
  int y = 29;
  int i;
  for (i = 0; i < 10; ++i)
  {
    if (x = i) /* HIT */
    {
      /* hooray! */
    }
  }
  if (x = y) /* HIT */
    printf("...");
  i = 0;
  while (i = 10) /* HIT */
  {
    printf("%d\n", i);
  }
  int c;
  while ((c = getchar())) /* no warning, because the assignment-expression is in parenthesis */
  {
    printf("%c\n", (char) c);
  }
  int x;
  for (x = 0, i = 0; i < 10; ++i)
  {
    x += i;
  }
}
