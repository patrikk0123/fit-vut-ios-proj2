/*
*IOS proj2
*author: Patrik Korytar, xkoryt04
*school: FIT VUT
*date: 24.04.2021
*module description: error codes and messages
*/

#ifndef __ERROR_H__
#define __ERROR_H__

// ERRROR CODES

#define RET_NOERR 0       // no error occured
#define RET_ERR 1         // error occured

#define ERR_NONE 0        // no error
#define ERR_ARG_COUNT 1   // bad arguments count (4 needed)
#define ERR_ARG_NOTNUM 2  // argument no a number
#define ERR_ARG_INVINT 3  // argument out of interval
#define ERR_SYSTEM 4      // error of operating system function - eg. sem_init

// error code variable - provide to other files
extern int err_var;

// print error var message
void print_error();
// print program syntax
void print_syntax();
// print allowed intervals of arguments
void print_intervals();

#endif // __ERROR_H__
