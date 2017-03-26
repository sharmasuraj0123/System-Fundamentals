#include "sfish.h"




char *builtins[NUMBER_OF_BUILTINS] = {
	"cd",
	"help",
	"pwd",
	"exit"
};

char *redirection[NUMBER_OF_REDIRECTIONS] ={
	"<",
	">",
	"|"
};


int sfish_analyze(char **cmd ,int cmdc,char* envp[]){

/*Method to check for Redirection symbols*/
int symbol = check_for_redirection_symbol(cmd, cmdc);
if (symbol > 0)
	return sfish_redirection(cmd,cmdc ,symbol, envp);

/*Loop to check for builtins first.*/
for(int i =0;i<NUMBER_OF_BUILTINS;i++){
	if(strcmp(cmd[0],builtins[i])==0)
		return sfish_builtin(cmd,i);
}

return  sfish_execute(cmd ,envp);
}

int sfish_redirection(char **cmd , int cmdc ,int first , char* envp[]){

	char symbol = *cmd[first];
	pid_t pid;
	int status;

	size_t args_size = (size_t)cmd[2] - (size_t)cmd[0];
  	pid = Fork();

  	if (pid == 0){
		/*There are only 5 possible cases of redirection :*/

	   /* Case I
		* Only one possible case
		* prog1 [ARGS] > output.txt
		*/
		if(symbol == '>'){
			/*Opening coppying and closing the output file*/
			int out;
			if((out = open(cmd[first+1], O_WRONLY
				| O_TRUNC | O_CREAT, S_IRUSR |
				S_IRGRP | S_IWGRP | S_IWUSR)) <0){
	  				perror("open");
					return 1;
	  			}
	  		dup2(out, 1);
	  		close(out);

        	char **args = malloc(MAX_SIZE*sizeof(char));
        	memcpy(args ,cmd,args_size);

        	/*Execute the program and exit the child & free the memory*/
	  		sfish_analyze(args,first,envp);
	  		if(args!=NULL)
	  			free(args);

	  		exit(0);
		}
		/* Case II & Case III
		* Two possible cases
		*/
		else if(symbol=='<'){

			int second = check_for_redirection_symbol(&(cmd[first +1]), cmdc-first-1);

			/*prog1 [ARGS] < input.txt > output.txt*/
			if(second > 0){
				second +=first +1;
				if(*cmd[second]== '>'){
					/*Opening coppying and closing the output file*/
					int out;
					if((out = open(cmd[second+1], O_WRONLY
						| O_TRUNC | O_CREAT, S_IRUSR |
						S_IRGRP | S_IWGRP | S_IWUSR)) <0){
				  		perror("open");
						return 1;
	  				}
	  				int in;
					if((in =open(cmd[first+1] , O_RDONLY))<0){
						perror("open");
						return 1;
					}
					/*Changing stdin and stdout.*/
					dup2(in, 0);
					dup2(out, 1);

	  				close(out);
					close(in);

					char **args = malloc(MAX_SIZE*sizeof(char));
        			memcpy(args ,cmd,args_size);

        			/*Execute the program and exit the child & free the memory*/
	  				sfish_analyze(args,first,envp);
	  				if(args!=NULL)
	  					free(args);

	  				exit(0);

				}
				else{
					/*To Do later*/
				}
			}
			/*prog1 [ARGS] < input.txt*/
			else{
				int in;
				if((in =open(cmd[first+1] , O_RDONLY))<0){
					perror("open");
					return 1;
				}
				dup2(in, 0);
				close(in);

				char **args = malloc(MAX_SIZE*sizeof(char));
        		memcpy(args ,cmd,args_size);

        		/*Execute the program and exit the child & free the memory*/
	  			sfish_analyze(args,first,envp);
	  			if(args!=NULL)
	  				free(args);

	  			exit(0);
			}

		}
	}else{
		// Parent process
    	do {
      		waitpid(pid, &status, WUNTRACED);
    	}while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}


	return 1;
}

int sfish_execute(char **cmd ,char* envp[]){

	pid_t pid;
	int status;

  	pid = Fork();
  	if (pid == 0){
	    /* Child process*/
	    /*It contains 2 types of excutable file:*/
	    /*Type 1: contains / in beggining, has complete path
	     * Nothing needs to be done.
	     */

  		/*Type 2: If the name of the executable does not contain a /,
	     *search the PATH environment variable for such an executable.
	    */
  		if(**cmd !='/'){
  			/*To check which of the path actually exists*/
	    	char * cursor = malloc(MAX_SIZE*sizeof(char));
	    	int count =0;

    		while(cursor != NULL){
    			strcpy(cursor,commonPaths[count]);
    			strcat(cursor,"/");
    			strcat(cursor,cmd[0]);

        		if(file_exist(cursor)){
        			break;
        		}
        		count++;
    		}
    		cmd[0] = cursor;

    		if(cursor!=NULL)
    			free(cursor);
  		}
  		if(file_exist(*cmd)){
	  		if (execve(*cmd, cmd,envp) <0)
		    printf("%s: Command not found.\n",*cmd);
		    /*This program will only return if any failure*/
		    exit(EXIT_FAILURE);
		}
		else{
		    	printf("%s: File doesn't Exist\n",*cmd);
		    }
  }else {
    // Parent process
    	do {
      		waitpid(pid, &status, WUNTRACED);
    	}while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }
return 1;
}

int sfish_builtin(char **cmd , int mode){
pid_t pid;
int status;
pid = Fork();

/*To perform all the cd operations*/
if(mode==0){
	char * tempPrev = malloc(sizeof(pwd));
	strcpy(tempPrev,prev_pwd);
	strcpy(prev_pwd,pwd);

	if(pid==0)
		exit(0);
	char *path  = cmd[1];
	//printf("path :%s\n",path);
		if (path==NULL){
			strcpy(pwd,"/home");
		}
		else if(strcmp(path ,"-") ==0){
			strcpy(pwd,tempPrev);
		}
		/*go to the previous directory*/
		else if(strcmp(path ,"..") ==0){
		int i = 0;
		for (i = strlen(pwd)-1; i>0;i--)
			if(pwd[i]=='/')
				break;
		//strncpy(path,pwd,i);
		pwd[i] ='\0';

		//strcpy(pwd, path);
		//printf("cd path: %s\n",pwd);
		}
		else if(*path != '/'){
			strcat(pwd, "/");
			strcat(pwd,path);
		}
		else{
			strcpy(pwd, path);
		}

	/*Execution of command*/
	//printf("%s\n",pwd);
	if (chdir(pwd) != 0 && pid!=0){
     	perror("cd: ");
     	strcpy(prev_pwd,tempPrev);
	}
	/* Free the memory*/
	if(tempPrev !=NULL)
		free(tempPrev);
}
/*To perform Help*/
else if(mode==1){
	if(pid==0)
		exit(0);
	HELP();
	}
/*PWD - showing the full path*/
else if(mode==2){

		if(pid ==0){
			printf("%s\n",pwd);
			exit(0);
		}
		else{
		// Parent process
	    	do {
	      		waitpid(pid, &status, WUNTRACED);
	    	}while (!WIFEXITED(status) && !WIFSIGNALED(status));
		}
	}
/*Exit case*/
else{
		if(pid==0)
			exit(0);
		exit(0);
	}

return 1;
}


void unix_error(char *msg)
{
fprintf(stderr,"%s: %s\n", msg, strerror(errno));
exit(0);
}

pid_t Fork(void){
pid_t pid;
if((pid = fork()) < 0)
unix_error("Fork error");
return pid;
}

int file_exist (char *filename)
{
  struct stat   buffer;
  return (stat (filename, &buffer) == 0);
}

char ** getCommonPaths(){

	char **commonPaths = malloc(MAX_SIZE*sizeof(char));
    char *cursor= getenv("PATH");
    commonPaths[0] = strtok(cursor ,":");
    int count =0;
    while(commonPaths[count] != NULL){
        //printf("%s\n",commonPaths[count]);
        count++;
        commonPaths[count]= strtok(NULL , ":");
    }

    return commonPaths;
}

int check_for_redirection_symbol(char **cmd, int cmdc){

/*Loop to check for Redirection symbols*/
	for (int i = 0; i < cmdc; i++)
		for(int j =0; j<NUMBER_OF_REDIRECTIONS;j++)
			if(strcmp(cmd[i],redirection[j])==0)
				return i;
	return 0;
}
