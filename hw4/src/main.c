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
    commonPaths = getCommonPaths();


    /*For signal Handling*/
    //struct sigaction sa;
    /*Initializing all the signals*/
    init_signals();
    Signal(SIGTSTP,SIG_IGN);

    if(getcwd(pwd ,sizeof(pwd))==NULL)
        perror("Incorrect Path");
    cmd_prompt = malloc(sizeof(pwd));
    strcat(cmd_prompt,"<sursharma> : <");
    strcat(cmd_prompt,pwd);
    strcat(cmd_prompt,"> $");

    while((cmd = readline(cmd_prompt)) != NULL) {


        char **args = malloc(MAX_SIZE*sizeof(char));

        args[0] = strtok(cmd ," ");
        int count =0;
        while(args[count++] != NULL)
            args[count]= strtok(NULL , " ");

        /*Block SigCHld before execution*/
        sigset_t mask, prev_mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGCHLD);
        sigprocmask(SIG_BLOCK, &mask, &prev_mask);

        if(args[0]!=NULL)
            sfish_analyze(args ,count-1,envp);

        //sigprocmask(SIG_SETMASK, &prev_mask, NULL);

        if(getcwd(pwd ,sizeof(pwd))==NULL)
            perror("Incorrect Path");

        strcpy(cmd_prompt,"<sursharma> : <");
        strcat(cmd_prompt,pwd);
        strcat(cmd_prompt,"> $");
         //All your debug print statements should use the macros found in debu.h
        /* Use the `make debug` target in the makefile to run with these enabled. */
        //info("Length of command entered: %ld\n", strlen(cmd));
        /* You WILL lose points if your shell prints out garbage values. */

    }
    /* Don't forget to free allocated memory, and close file descriptors.
     */
    if(commonPaths!=NULL)
        free(commonPaths);
    if(cmd_prompt!=NULL)
        free(cmd_prompt);
    free(cmd);

    return EXIT_SUCCESS;
}

