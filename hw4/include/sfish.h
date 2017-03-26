#ifndef SFISH_H
#define SFISH_H
#include <readline/readline.h>
#include <readline/history.h>

#include <stdlib.h>
#include <stdio.h>


#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAX_SIZE 1024
#define NUMBER_OF_BUILTINS 4
#define NUMBER_OF_REDIRECTIONS 3


/*Declared pwd outside so that it can be accessed by all*/
char pwd[MAX_SIZE];
char prev_pwd[MAX_SIZE];
char **commonPaths;

/*For safe wrapping around fork*/
void unix_error(char *msg);
pid_t Fork(void);

/*Helper Methods*/

int file_exist (char *filename);
char ** getCommonPaths();

/* Returns the first instance of
 * rediection symbol in the array.
 */
int check_for_redirection_symbol(char **cmd, int cmdc);


/*
* This function is the first step of the shell
* Where it analyzes it as built in or a program;
* And will call teh functions appropriately.
*/
int sfish_analyze(char **cmd ,int cmdc,char* envp[]);
/*
* This function is to excute the program in the shell.
*/
int sfish_execute(char **cmd ,char* envp[]);
/*
* This function is to handle builtins.
* The mode will specify which built in function is called.
*/
int sfish_builtin(char **cmd , int mode);
/*
*This function will handle all the inputs with redirection
* @param cmdc : the number of arguments.
* @param first : is the first appeared instance of redirections
*/
int sfish_redirection(char **cmd , int cmdc , int first , char* envp[]);



#define HELP() do{                                                        \
fprintf(stdout, "%s\n",                                                           \
"usage: ./hw1 [-s | -t | -h]\n"                                                   \
"    -s       Substitution cipher\n"                                              \
"             Additional parameters: [-e | -d] n INPUT_FILE OUTPUT_FILE\n"        \
"                 -e           Encrypt using the substitution cipher.\n"          \
"                 -d           Decrypt using the substitution cipher.\n"          \
"                  n           The amount of position to shift by.\n"             \
"                  INPUT_FILE  This can be any file on the file system or '-'\n"  \
"                              which specifies stdin.\n"                          \
"                  OUTPUT_FILE This can be any file on the system or '-'\n"       \
"                              which specifies stdout.\n"                         \
"\n"                                                                              \
"    -t       Tutnese Translation\n"                                              \
"             Additional parameters: [-e | -d]   INPUT_FILE OUTPUT_FILE\n"        \
"                 -e          Encode into tutnese\n"                              \
"                 -d          Decode from tutnese (extra credit)\n"               \
"                 INPUT_FILE  This can be any file on the file system or '-'\n"   \
"                             which specifies stdin\n"                            \
"                 OUTPUT_FILE This can be any file on the system or '-'\n"        \
"                             which specifies stdout.\n"                          \
"\n"                                                                              \
"    -h       Display this help menu.");                                          \
} while(0)

#endif
