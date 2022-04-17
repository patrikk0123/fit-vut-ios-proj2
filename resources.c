/*
*IOS proj2
*author: Patrik Korytar, xkoryt04
*school: FIT VUT
*date: 24.04.2021
*module description: creation and deletion of reources (shared mem, semaphores, etc.)
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <unistd.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/types.h>

#include "resources.h"
#include "process.h"
#include "params.h"
#include "error.h"

// RESOURCES USED IN PROGRAM

// parameters structure
params_t params;
// file to write output to - file descriptors are inherited, so every process needs to close it
FILE *file;
// pointer to shared memory
shared_t *shm_p;
// process id of santa process
pid_t santa_pid;
// process ids of elf processes -- saved in shared mem to prevent subprocess inheritance
pid_t *elf_pids;
// process ids of rd processes -- saved in shared mem to prevent subprocess inheritance
pid_t *rd_pids;

// open output file, shared mem, private shared mem for elf and rd PIDs and initialize semaphores
int init_resources()
{
  file = fopen("proj2.out", "w");
  if(file == NULL)
    goto end;

  shm_p = mmap(NULL, sizeof(shared_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  if(shm_p == MAP_FAILED)
    goto close_file;
  elf_pids = mmap(NULL, sizeof(pid_t) * params.elf_cnt, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if(elf_pids == MAP_FAILED)
    goto unmap_shm;
  rd_pids = mmap(NULL, sizeof(pid_t) * params.rd_cnt, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if(rd_pids == MAP_FAILED)
    goto unmap_eid;

  init_shm();
  if(init_sems() == RET_ERR)
    goto unmap_rid;
  init_pid_arr(elf_pids, params.elf_cnt);
  init_pid_arr(rd_pids, params.rd_cnt);

  return RET_NOERR;

  // if failed, destroy what created
  unmap_rid:  munmap(rd_pids, sizeof(pid_t) * params.rd_cnt);
  unmap_eid:  munmap(elf_pids, sizeof(pid_t) * params.elf_cnt);
  unmap_shm:  munmap(shm_p, sizeof(shared_t));
  close_file: fclose(file);
  end:        err_var = ERR_SYSTEM;
  return RET_ERR;
}

// initialize shared memory variables
void init_shm()
{
  shm_p->line = 0;
  shm_p->elf_help_cnt = 0;
  shm_p->rd_hitch_cnt = 0;
  shm_p->closed = false;

  return;
}

// initialize all semaphores
int init_sems()
{
  if(sem_init(&shm_p->sem_print, 1, 1)        == -1) goto del0;
  if(sem_init(&shm_p->sem_mutex, 1, 1)        == -1) goto del1;
  if(sem_init(&shm_p->sem_santa_sleep, 1, 0)  == -1) goto del2;
  if(sem_init(&shm_p->sem_santa_wait, 1, 0)   == -1) goto del3;
  if(sem_init(&shm_p->sem_elf_help, 1, 1)     == -1) goto del4;
  if(sem_init(&shm_p->sem_elf_wait, 1, 0)     == -1) goto del5;
  if(sem_init(&shm_p->sem_elf_endhelp, 1, 0)  == -1) goto del6;
  if(sem_init(&shm_p->sem_rd_hitch, 1, 0)     == -1) goto del7;
  return RET_NOERR;

  // if failed, destroy other already initialized semaphores
  del7: sem_destroy(&shm_p->sem_elf_endhelp);
  del6: sem_destroy(&shm_p->sem_elf_wait);
  del5: sem_destroy(&shm_p->sem_elf_help);
  del4: sem_destroy(&shm_p->sem_santa_wait);
  del3: sem_destroy(&shm_p->sem_santa_sleep);
  del2: sem_destroy(&shm_p->sem_mutex);
  del1: sem_destroy(&shm_p->sem_print);
  del0:
  return RET_ERR;
}

// increase value of semaphore by count
void increase_sem(sem_t *sem, int count)
{
  for(int i = 0; i < count; i++)
    sem_post(sem);

  return;
}

// destroy all semaphores
void destroy_sems()
{
  sem_destroy(&shm_p->sem_print);
  sem_destroy(&shm_p->sem_mutex);
  sem_destroy(&shm_p->sem_santa_sleep);
  sem_destroy(&shm_p->sem_santa_wait);
  sem_destroy(&shm_p->sem_elf_help);
  sem_destroy(&shm_p->sem_elf_wait);
  sem_destroy(&shm_p->sem_elf_endhelp);
  sem_destroy(&shm_p->sem_rd_hitch);

  return;
}

// initialize array of PIDs to zero
void init_pid_arr(pid_t *arr, int count)
{
  for(int i = 0; i < count; i++)
    arr[i] = 0;

  return;
}

// generator of santa process
int create_santa()
{
  // one santa will be created
  santa_pid = fork(); // save PID
  if(santa_pid == -1) // if failed
  {
    err_var = ERR_SYSTEM;
    return RET_ERR;
  }
  if(santa_pid == 0) // this is santa
  {
    santa_func(); // do his work
    fclose(file); // close file - every process needs to do that
    exit(0);      // end process
  }

  // main process continues
  return RET_NOERR;
}

// generator of elf processes
int create_elves()
{
  // given number of elves will be created
  for(int i = 0; i < params.elf_cnt; i++)
  {
    elf_pids[i] = fork(); // save PIDs
    if(elf_pids[i] == -1) // if failed
    {
      err_var = ERR_SYSTEM;
      return RET_ERR;
    }
    if(elf_pids[i] == 0) // this is elf
    {
      srand(time(NULL) * getpid() * 2); // set unique random num generator seed
      elf_func(i + 1);  // do his work - elf has his id
      fclose(file);     // close file - every process needs to do that
      exit(0);          // end process
    }
  }

  // main process continues
  return RET_NOERR;
}

// generator of rd processes
int create_rds()
{
  // given number of rds will be created
  for(int i = 0; i < params.rd_cnt; i++)
  {
    rd_pids[i] = fork(); // save PIDs
    if(rd_pids[i] == -1) // if failed
    {
      err_var = ERR_SYSTEM;
      return RET_ERR;
    }
    if(rd_pids[i] == 0) // this is rd
    {
      srand(time(NULL) * getpid() * 4); // set unique random num generator seed
      rd_func(i + 1); // do his work - rd has his id
      fclose(file);   // close file - every process needs to do that
      exit(0);        // end process
    }
  }

  // main process continues
  return RET_NOERR;
}

// kill all processes given by array
void kill_processes(pid_t *pid, int count)
{
  // array of pid is initalized to 0, so on id 0 will stop
  for(int i = 0; i < count && pid[i] != 0; i++)
    kill(pid[i], SIGTERM);

  return;
}
