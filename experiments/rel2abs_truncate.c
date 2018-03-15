#include <stdio.h>

int main(int argc, char *argv[argc])
{
  char c, last_c;
  double rel_time, last_abs_time;
  double abs_time = 0;
  int truncating = 0;
  while (scanf(" %c %lf\n",&c, &rel_time) != EOF) {
    abs_time += rel_time;

    if (c == '_') {
      if (!truncating) {
        printf("%c %10lf\n", c, abs_time);
      }
      truncating = 1;
      last_c = c;
      last_abs_time = abs_time;
    } else {
      if (truncating) {
        printf("%c %10lf\n", last_c, last_abs_time);
        truncating = 0;
      }
      printf("%c %10lf\n", c, abs_time);      
    }
  }
  return 0;
}
