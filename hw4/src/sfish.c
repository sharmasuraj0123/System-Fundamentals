#include "sfish.h"




char *builtins[NUMBER_OF_BUILTINS] = {
	"cd",
	"help",
	"pwd",
	"exit"
};


int sfish_analyze(char **cmd){


char * operation =strtok(*cmd," ");

/*Loop to check for builtins first.*/
for(int i =0;i<NUMBER_OF_BUILTINS;i++){
	if(strcmp(operation,builtins[i])==0)
		return sfish_builtin(cmd,i);
}


return 0;
}

int sfish_execute(char **cmd){
return 0;
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
	char *path  = strtok(NULL," ");
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
		HELP(0);
		if(pid==0)
			exit(0);
	}
else if(mode==2){

		if(pid ==0){
			fprintf(stderr ,"%s\n",pwd);
			exit(0);
		}
		else{
		wait(&child_status);
		}
	}
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




