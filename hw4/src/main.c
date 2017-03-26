#include "sfish.h"
#include "debug.h"
#include <errno.h>
/*
 * As in previous hws the main function must be in its own file!
 */

int main(int argc, char const *argv[], char* envp[]){
    /* DO NOT MODIFY THIS. If you do you will get a ZERO. */
    rl_catch_signals = 0;
    /* This is disable readline's default signal handlers, since you are going to install your own.*/
    char *cmd;

    if(getcwd(pwd ,sizeof(pwd))==NULL)
        perror("Incorrect Path");
    char * a = malloc(sizeof(pwd));
    strcat(a,"<sursharma> : <");
    strcat(a,pwd);
    strcat(a,"> $");

    while((cmd = readline(a)) != NULL) {


        /*Analyzing the shell and all tis commands*/
        sfish_analyze(&cmd);


        if(getcwd(pwd ,sizeof(pwd))==NULL)
            perror("Incorrect Path");

        strcpy(a,"<sursharma> : <");
        strcat(a,pwd);
        strcat(a,"> $");
         //All your debug print statements should use the macros found in debu.h
        /* Use the `make debug` target in the makefile to run with these enabled. */
        //info("Length of command entered: %ld\n", strlen(cmd));
        /* You WILL lose points if your shell prints out garbage values. */
    }

    /* Don't forget to free allocated memory, and close file descriptors.
     */
    if(a!=NULL)
        free(a);
    free(cmd);

    return EXIT_SUCCESS;
}


