/*
*IOS proj2
*author: Patrik Korytar, xkoryt04
*school: FIT VUT
*date: 24.04.2021
*module description: creation and deletion of reources (shared mem, semaphores, etc.)
*/

#ifndef __RESOURCES_H__
#define __RESOURCES_H__

#include <stdio.h>
#include <stdbool.h>

#include <semaphore.h>
#include <sys/types.h>

#include "params.h"

typedef struct
{
  int line;               // number of line in output file
  int elf_help_cnt;       // number of elves getting helped by santa - max 3
  int rd_hitch_cnt;       // number of reindeers returned home
  bool closed;            // Christmas started

  // SEMAPHORES
  sem_t sem_print;        // message printing
  sem_t sem_mutex;        // used on many places
  sem_t sem_santa_sleep;  // santa sleeping (waiting for elves or reindeers)
  sem_t sem_santa_wait;   // santa waiting for reindeers to get hitched
  sem_t sem_elf_help;     // elves going to get help
  sem_t sem_elf_wait;     // elves waiting for help
  sem_t sem_elf_endhelp;  // elves ending help
  sem_t sem_rd_hitch;     // reindeers getting hitched
} shared_t;

// RESOURCES USED IN PROGRAM - provide to other files

// parameters structure
extern params_t params;
// file to write output to - file descriptors are inherited, so every process needs to close it
extern FILE *file;
// pointer to shared memory
extern shared_t *shm_p;
// process id of santa process
extern pid_t santa_pid;
// process ids of elf processes -- saved in shared mem to prevent subprocess inheritance
extern pid_t *elf_pids;
// process ids of rd processes -- saved in shared mem to prevent subprocess inheritance
extern pid_t *rd_pids;

// open output file, shared mem, private shared mem for elf and rd PIDs and initialize semaphores
int init_resources();
// initialize shared memory variables
void init_shm();
// initialize all semaphores
int init_sems();
// increase value of semaphore by count
void increase_sem(sem_t *sem, int count);
// destroy all semaphores
void destroy_sems();
// initialize array of PIDs to zero
void init_pid_arr(pid_t *arr, int count);
// generator of santa process
int create_santa();
// generator of elf processes
int create_elves();
// generator of rd processes
int create_rds();
// kill all processes given by array
void kill_processes(pid_t *pid, int count);

#endif // __RESOURCES_H__
