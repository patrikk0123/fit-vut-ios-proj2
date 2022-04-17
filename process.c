/*
*IOS proj2
*author: Patrik Korytar, xkoryt04
*school: FIT VUT
*date: 24.04.2021
*module description: process code of santa, elves and reindeers
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <unistd.h>
#include <semaphore.h>

#include "process.h"
#include "resources.h"
#include "error.h"

// print message to output file
void print_msg(char *name, int id, char *msg)
{
  sem_wait(&shm_p->sem_print); // max one at the time

    shm_p->line++; // increase line number

    if(id == NO_ID) // santa has no ID
      fprintf(file, "%d: %s: %s\n", shm_p->line, name, msg);
    else
      fprintf(file, "%d: %s %d: %s\n", shm_p->line, name, id, msg);

    fflush(file); // print it immediately

  sem_post(&shm_p->sem_print);

  return;
}

// work of santa
void santa_func()
{
  while(true) // repeat - ended by break
  {
    print_msg("Santa", NO_ID, "going to sleep");
    sem_wait(&shm_p->sem_santa_sleep); // santa sleeps - waits for elves or rds

    sem_wait(&shm_p->sem_mutex); // sync protection

      if(shm_p->rd_hitch_cnt == params.rd_cnt) // all reindeers have come
      {
        print_msg("Santa", NO_ID, "closing workshop");
        increase_sem(&shm_p->sem_rd_hitch, shm_p->rd_hitch_cnt);  // all rds will get hitched

        sem_post(&shm_p->sem_mutex);

        sem_wait(&shm_p->sem_santa_wait);                         // wait while rds getting hitched
        print_msg("Santa", NO_ID, "Christmas started");
        shm_p->closed = true;                                     // Christmas started
        increase_sem(&shm_p->sem_elf_wait, shm_p->elf_help_cnt);  // allow all waiting elves go on holidays

        break; // santa goes on holidays
      }
      else if(shm_p->elf_help_cnt == 3) // three elves need help
      {
        print_msg("Santa", NO_ID, "helping elves");
        increase_sem(&shm_p->sem_elf_wait, 3);  // all waiting elves will get help

        sem_post(&shm_p->sem_mutex);

        sem_wait(&shm_p->sem_elf_endhelp);      // wait while elves getting help
      }
  }

  return;
}

// work of elves
void elf_func(int elfID)
{
  print_msg("Elf", elfID, "started");

  while(true) // repeat - ended by break
  {
    usleep((rand()%(params.elf_maxtime + 1)) * 1000); // elf works
    print_msg("Elf", elfID, "need help");

    sem_wait(&shm_p->sem_elf_help); // allow only three elves pass

    sem_wait(&shm_p->sem_mutex); // sync protection

      shm_p->elf_help_cnt++;                // elf needs help
      if(shm_p->elf_help_cnt == 3)          // third elf
        sem_post(&shm_p->sem_santa_sleep);  // wake up santa
      else                                  // first and second elf
        sem_post(&shm_p->sem_elf_help);     // allow another elf pass

    sem_post(&shm_p->sem_mutex);

    if(shm_p->closed) // if elf comes after Christmas started
    {
      sem_post(&shm_p->sem_elf_help); // allow another elf pass
      print_msg("Elf", elfID, "taking holidays");
      break; // elf goes on holidays
    }

    sem_wait(&shm_p->sem_elf_wait); // elf waits for help

    if(shm_p->closed) // if elf is before Christmas started
    {
      sem_post(&shm_p->sem_elf_help); // allow another elf pass
      print_msg("Elf", elfID, "taking holidays");
      break; // elf goes on holidays
    }

    print_msg("Elf", elfID, "get help");

    sem_wait(&shm_p->sem_mutex); // sync protection

      shm_p->elf_help_cnt--;        // elf goes away
      if(shm_p->elf_help_cnt == 0)  // last elf
      {
        sem_post(&shm_p->sem_elf_endhelp);  // notify santa that he can go sleep
        sem_post(&shm_p->sem_elf_help);     // allow another elf pass
      }

    sem_post(&shm_p->sem_mutex);
  }

  return;
}

// work of reindeers
void rd_func(int rdID)
{
  print_msg("RD", rdID, "rstarted");

  usleep((rand() % (params.rd_maxtime/2 + 1) + params.rd_maxtime/2) * 1000); // rd comes home
  print_msg("RD", rdID, "return home");

  sem_wait(&shm_p->sem_mutex); // sync protection

    shm_p->rd_hitch_cnt++;                    // rd comes home
    if(shm_p->rd_hitch_cnt == params.rd_cnt)  // last rd
      sem_post(&shm_p->sem_santa_sleep);      // wake up santa

  sem_post(&shm_p->sem_mutex);

  sem_wait(&shm_p->sem_rd_hitch); // wait for getting hitched
  print_msg("RD", rdID, "get hitched");

  sem_wait(&shm_p->sem_mutex); // sync protection

    shm_p->rd_hitch_cnt--;              // rd goes on holidays
    if(shm_p->rd_hitch_cnt == 0)        // last rd
      sem_post(&shm_p->sem_santa_wait); // notify santa that he can go on holidays

  sem_post(&shm_p->sem_mutex);

  return;
}
