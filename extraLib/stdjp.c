#include <string.h>
#include "stdjp.h"

/**
 * reverses a string
 * @param s string to reverse
 */
void reverse(char s[]) {
  int i, j;
  char c;

  for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
    c = s[i];
    s[i] = s[j];
    s[j] = c;
  }
}

/**
 * convert number to string from K&R book, largest number length == 6
 * @param  n number to convert to a string
 * @param  s buffer to contain string
 * @return   char array of number
 */
void itoa(int n, char s[]) {
  int i, sign;

  if ((sign = n) < 0)  /* record sign */
     n = -n;          /* make n positive */
  i = 0;
  do {       /* generate digits in reverse order */
     s[i++] = n % 10 + '0';   /* get next digit */
  } while ((n /= 10) > 0);     /* delete it */
  if (sign < 0)
     s[i++] = '-';
  s[i] = '\0';
  reverse(s);
}
