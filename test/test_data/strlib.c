#include <stdio.h>

int print_i64(int num) {
  printf("%d", num);
  return 0;
}
int print_f64(double num) {
  printf("%f", num);
  return 0;
}
int print_char(char c) {
  printf("%c", c);
  return 0;
}
int print_str(char *str) {
  printf("%s", str);
  return 0;
}
int print_nl(char *str) {
  printf("\n");
  return 0;
}
