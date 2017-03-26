#include "sfish.h"




char *builtins[NUMBER_OF_BUILTINS] = {
	"cd",
	"help",
	"pwd",
	"exit"
};


int sfish_analyze(char **cmd ,char* envp[]){

/*Loop to check for builtins first.*/
for(int i =0;i<NUMBER_OF_BUILTINS;i++){
	if(strcmp(cmd[0],builtins[i])==0)
		return sfish_builtin(cmd,i);
}

return  sfish_execute(cmd ,envp);
}

int sfish_execute(char **cmd ,char* envp[]){

	pid_t pid;
	int status;

  	pid = Fork();
  	if (pid == 0){
	    /* Child process*/
	    /*It contains 2 types of excutable file:*/
	    /*Type 1: contains / in beggining, has complete path*/

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
int child_status;
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
			fprintf(stderr ,"%s\n",pwd);
			exit(0);
		}
		else{
		wait(&child_status);
		}
	}
/*Exit case*/
else{
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

