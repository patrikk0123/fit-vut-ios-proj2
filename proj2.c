/*
*IOS proj2
*author: Patrik Korytar, xkoryt04
*school: FIT VUT
*date: 24.04.2021
*module description: main program
*/

#include <stdio.h>
#include <stdlib.h>

#include <signal.h>
#include <sys/mman.h>
#include <sys/wait.h>

#include "resources.h"
#include "params.h"
#include "error.h"

// SIGTERM signal handler
void term_func(int pid);

int main(int argc, char **argv)
{
  // load parameters in params_t structure
  if(load_params(&params, argc, argv) == RET_ERR)
    goto end;

  // get resources - file, shared mem, elf and rd PID private shared mem, semaphores
  if(init_resources() == RET_ERR)
    goto end;

  // change SIGTERM signal handler - used to kill subprocesses - need to close file
  signal(SIGTERM, term_func);

  // create santa, elf and rd processes
  if(create_santa() == RET_ERR)
    goto del_sems;
  if(create_elves() == RET_ERR)
    goto kill_elves;
  if(create_rds() == RET_ERR)
    goto kill_rds;

  while(wait(NULL) > 0); // wait for subprocesses to end
  goto del_sems; // subprocesses ended, skip killing them

  // free resources
  kill_rds:   kill_processes(rd_pids, params.rd_cnt);
  kill_elves: kill_processes(elf_pids, params.elf_cnt);
              kill(santa_pid, SIGTERM);
  del_sems:   destroy_sems();
              munmap(rd_pids, sizeof(pid_t) * params.rd_cnt);
              munmap(elf_pids, sizeof(pid_t) * params.elf_cnt);
              munmap(shm_p, sizeof(shared_t));
              fclose(file);
  end:

  if(err_var != ERR_NONE) // if error detected
  {
    print_error();  // print error message
    return RET_ERR; // return 1
  }

  return RET_NOERR; // return 0
}

// SIGTERM signal handler
void term_func(int pid)
{
  (void)pid;
  fclose(file);

  exit(1);
}
