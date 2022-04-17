/*
*IOS proj2
*author: Patrik Korytar, xkoryt04
*school: FIT VUT
*date: 24.04.2021
*module description: processing of parameters
*/

#include <stdlib.h>

#include "params.h"
#include "error.h"

// load parameters and return it as a params_t structure
int load_params(params_t *params, int argc, char **argv)
{
  if(argc < 5 || argc > 5)
  {
    err_var = ERR_ARG_COUNT;
    return RET_ERR;
  }

  for(int i = 1; i < argc; i++) // argument 0 is file path
  {
    char *p = NULL;
    int num = strtoul(argv[i], &p, 10);
    if(p[0] != '\0') // whole argument is not a number
    {
      err_var = ERR_ARG_NOTNUM;
      return RET_ERR;
    }

    arg_write(i, num, params);
  }

  if(  params->elf_cnt <= 0 || params->elf_cnt >= 1000
    || params->rd_cnt <= 0 || params->rd_cnt >= 20
    || params->elf_maxtime < 0 || params->elf_maxtime > 1000
    || params->rd_maxtime < 0 || params->rd_maxtime > 1000)
  {
    err_var = ERR_ARG_INVINT;
    return RET_ERR;
  }

  return RET_NOERR;
}

// write argument in params_t sctructure
void arg_write(int param_no, int num, params_t *params)
{
  // save argument according to its position
  switch(param_no)
  {
    case 1:
      params->elf_cnt = num;
      break;
    case 2:
      params->rd_cnt = num;
      break;
    case 3:
      params->elf_maxtime = num;
      break;
    case 4:
      params->rd_maxtime = num;
  }

  return;
}
