/*
*IOS proj2
*author: Patrik Korytar, xkoryt04
*school: FIT VUT
*date: 24.04.2021
*module description: process code of santa, elves and reindeers
*/

#ifndef __PROCESS_H__
#define __PROCESS_H__

#define NO_ID -1

// print message to output file
void print_msg(char *name, int id, char *msg);
// work of santa
void santa_func();
// work of elves
void elf_func(int elfID);
// work of reindeers
void rd_func(int rdID);

#endif // __PROCESS_H__
