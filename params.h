/*
*IOS proj2
*author: Patrik Korytar, xkoryt04
*school: FIT VUT
*date: 24.04.2021
*module description: processing of parameters
*/

#ifndef __PARAMS_H__
#define __PARAMS_H__

typedef struct
{
  int elf_cnt;      // max count of elves
  int rd_cnt;       // max count of reindeers
  int elf_maxtime;  // elf working max time
  int rd_maxtime;   // reindeer coming home max time
} params_t;

// load parameters and return it as a params_t structure
int load_params(params_t *params, int argc, char **argv);
// write argument in params_t sctructure
void arg_write(int param_no, int num, params_t *params);

#endif // __PARAMS_H__
