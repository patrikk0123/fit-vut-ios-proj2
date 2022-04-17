/*
*IOS proj2
*author: Patrik Korytar, xkoryt04
*school: FIT VUT
*date: 24.04.2021
*module description: error codes and messages
*/

#include <stdio.h>

#include "error.h"

// error code variable
int err_var = ERR_NONE;

// print error var message
void print_error()
{
  fprintf(stderr, "proj2: ");

  switch(err_var)
  {
    case ERR_ARG_COUNT:
      fprintf(stderr, "bad arguments count - 4 needed\n");
      print_syntax();
      break;
    case ERR_ARG_NOTNUM:
      fprintf(stderr, "not a number given as an argument\n");
      break;
    case ERR_ARG_INVINT:
      fprintf(stderr, "argument out of interval\n");
      print_intervals();
      break;
    case ERR_SYSTEM:
      fprintf(stderr, "internal program error\n");
  }

  return;
}

// print program syntax
void print_syntax()
{
  fprintf(stderr, "SYNTAX: ./proj2 NE NR TE TR\n");
  fprintf(stderr, "   NE - count of elves\n");
  fprintf(stderr, "   NR - count of reindeers\n");
  fprintf(stderr, "   TE - max work time of an elf (in miliseconds)\n");
  fprintf(stderr, "   TE - max time after which reindeer returns home (in miliseconds)\n");

  return;
}

// print allowed intervals of arguments
void print_intervals()
{
  fprintf(stderr, "INTERVALS OF ARGUMENTS:\n");
  fprintf(stderr, "   0 < NE < 1000\n");
  fprintf(stderr, "   0 < NR < 20\n");
  fprintf(stderr, "   0 <= TE <= 1000\n");
  fprintf(stderr, "   0 <= RE <= 1000\n");

  return;
}
