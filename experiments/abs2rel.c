#include <stdio.h>

int main(int argc, char *argv[argc])
{
  char c;
  double abs_time, last_abs_time = 0;
  while (scanf(" %c %lf\n",&c, &abs_time) != EOF) {
    printf("%c %10lf\n", c, abs_time - last_abs_time);
    last_abs_time = abs_time;
  }
  return 0;
}
