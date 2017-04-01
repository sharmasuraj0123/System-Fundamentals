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

#include <signal.h>

#define MAX_SIZE 1024
#define NUMBER_OF_BUILTINS 5
#define NUMBER_OF_REDIRECTIONS 3


/*Declared pwd outside so that it can be accessed by all*/
char pwd[MAX_SIZE];
char prev_pwd[MAX_SIZE];
char **commonPaths;
pid_t pid;
int alarm_time;
char * cmd_prompt;

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

/*Methods for signals*/
void handler(int sig);
void init_signals();

typedef void (handler_t)(int);
handler_t *Signal(int signum, handler_t *handler);

/*
* This function is the first step of the shell
* Where it analyzes it as built in or a program;
* And will call the functions appropriately.
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



#define HELP() do{                                                        		\
fprintf(stdout, "%s\n",                                                           \
"usage: ./hw4 Shell Sfish - Builtins\n"                                                   		\
"	cd [-L|[-P [-e]] [-@]] [dir]       		Changes Directory\n"                          \
"             Additional parameters: [-L|[-P [-e]] [-@]] [dir]\n"       			\
"                 ..        Takes to directory just before.\n"          			\
"                 .         Keeps in same Directory\n"          						\
"                 -         Takes to previous Working directory\n"             			\
"                 [dir]		The path of the director you want to switch to.\n"  		\
"	alarm [n] 		Sets a timer for [n] seconds\n" 					\
"             Additional parameters: [n]\n"        										\
"                 [n]        Total time in seconds.\n"          						\
"	pwd [-LP]       prints the absolute path of the current working directory\n" 		\
"	exit [] 		exits the program\n"    											\
"	help       Display this help menu.");                                          	\
} while(0)
#endif
