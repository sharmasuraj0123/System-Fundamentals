#include "server_helper.h"


void *login_thread(void * vargp){

Pthread_detach(pthread_self());
while(1){
	int *connfd = remove_index_al(login , 0);
	if(connfd!=NULL)
		if(verifyLogin(*connfd)){
			//communicate(*connfd);
			//client *testClient = get_index_al(p->users,0);
			//printf("%s\n",testClient->username);
			//printf("%d\n",testClient->fd);
		}
}
}

void* communication_thread(void* vargp){

	Pthread_detach(pthread_self());

	while(1){
		if(p->users->length !=0){
			client *active = malloc(sizeof(client));
			/*Check for disconnected clients*/

			// for(int i=0; i<p->users->length;i++){
			// 	active = get_index_al(p->users,i);
			// 	/*Check Whether User is still connected*/
			// 	/*Just to confirm again*/
			// 	if(!valid_fd(active->fd)){
			// 			printf("YES IT GOES\n");
			// 			// FD_CLR(active->fd,&p->read_set);
			// 			// remove_index_al(p->users, i);
			// 			// remove_index_al(usernames,i);
			// 			// update_max(p);
			// 			// printf("%i\n",p->maxfd);
			// 			continue;
			// 			/*Free the memory*/
			// 	}
			// }
			int i;
			struct timeval timeout;

			timeout.tv_sec =1;
			timeout.tv_usec= 0;

			p->ready_set = p->read_set;
			p->nready = Select(p->maxfd+1,&p->ready_set,NULL,NULL,&timeout);

		if(p->nready>0){
			//printf("number ready: %d\n",p->nready);
			/*Check for STDIN in server*/
			if(FD_ISSET(STDIN_FILENO,&p->ready_set) && !stdin_lock){
				//printf("HELLO\n");
				pthread_t tid;
				Pthread_create(&tid,NULL,broadcast_thread,NULL);
				p->nready--;
			}

			for(i=0; i<p->users->length;i++){
				active = get_index_al_2(p->users,i);

				if(FD_ISSET(active->fd,&p->ready_set) && !active->lock){
					//printf("User :%s : %i\n",active->username,active->fd);
					pthread_t tid;
					Pthread_create(&tid,NULL,broadcast_thread,active);
					p->nready--;
				}
			}

		}

		}

	}
}

void* broadcast_thread(void* vargp){

	//Pthread_detach(pthread_self());
	//while(1){
	char buf[MAXLINE];
	int read_size;
	char buf2[MAXLINE];

	/*For Server Commands*/
	if(vargp==NULL){

		if(stdin_lock){
			Pthread_exit(0);
			return NULL;
		}

		rio_t stdin_rio;
		stdin_lock = true;
		Rio_readinitb(&stdin_rio,STDIN_FILENO);
		read_size =Rio_readlineb(&stdin_rio,buf,MAXLINE);

		if(strncmp(buf,MSG,4)==0){
			for(int i=0; i<p->users->length;i++){
				char *data = &buf[4];
				client *all_client = get_index_al(p->users,i);
				/*The Message Sent to all users*/
				Rio_writen(all_client->fd,MSG,sizeof(MSG));
				Rio_writen(all_client->fd,SERVER,strlen(SERVER));
				Rio_writen(all_client->fd,COLON,sizeof(COLON));
				Rio_writen(all_client->fd,data,sizeof(data));
			}
		}
		else if(strcmp(buf,"WHO\n")==0){
			for(int i=0; i<p->users->length;i++){
					client *all_client = get_index_al(p->users,i);
					/*To extract actual name & fd*/
					char * nameofClient = malloc(sizeof(all_client->username));
					strncpy(nameofClient,all_client->username,strlen(all_client->username)-2);
					/*Printing to stdout*/
					Rio_writen(STDOUT_FILENO,nameofClient,sizeof(nameofClient));
					Rio_writen(STDOUT_FILENO,SPACE,sizeof(SPACE));
					Rio_writen(STDOUT_FILENO,COLON,sizeof(COLON));
					Rio_writen(STDOUT_FILENO,SPACE,sizeof(SPACE));
					fprintf(stdout, "%i\n",all_client->fd);
					Rio_writen(STDOUT_FILENO,NEWLINE,sizeof(NEWLINE));
					Free(nameofClient);
				}
		}
		else{
			/*Not Specified*/
			//printf("SHOULD NOT GO HERE1\n");
		}
		stdin_lock = false;
		Pthread_exit(0);
		return NULL;
	}

		client* active = (client *) vargp;
		if(active->lock){
			//printf("HUH!\n");
			Pthread_exit(0);
			return NULL;
		}
		/*Locking before Execution*/
		active->lock = true;
		if((read_size =Rio_readlineb(&active->rio,buf,MAXLINE))!=0){
			read_size = Rio_readlineb(&active->rio,buf2,MAXLINE);

			if(strncmp(buf,MSG,4)==0){
				char *data = &buf[4];
				size_t active_index = get_data_al(usernames,active->username);

				/*To extract actual name*/
				char nameofClient[MAXLINE];
				strncpy(nameofClient,active->username,strlen(active->username)-2);

				for(int i=0; i<p->users->length;i++){
					client *all_client = get_index_al(p->users,i);
					if(i!=active_index){
						/*The Message Sent to all other users*/
						Rio_writen(all_client->fd,MSG,sizeof(MSG));
						Rio_writen(all_client->fd,nameofClient,strlen(nameofClient));
						Rio_writen(all_client->fd,COLON,sizeof(COLON));
						Rio_writen(all_client->fd,data,sizeof(data));
						Rio_writen(all_client->fd,NEWLINE,sizeof(NEWLINE));
					}
				}
				/*Message Sent to current User After Completion*/
					Rio_writen(active->fd,GSM,sizeof(GSM));
					Rio_writen(active->fd,data,sizeof(data));
					Rio_writen(active->fd,NEWLINE,sizeof(NEWLINE));

			}
			else if(strcmp(buf,WHO)==0){
				Rio_writen(active->fd,OHW,sizeof(OHW));
				for(int i=0; i<usernames->length;i++){
					char * name = get_index_al(usernames,i);
					Rio_writen(active->fd,name,sizeof(name));
				}
			}
			else{
				/*Not specified*/
				//printf("SHOULD NOT GO HERE\n");
			}
			/*For unlocking again after execution*/
			active->lock = false;

		}else{

			/*Client is no longer responsive*/
			/*So removing him*/
			// printf("SHOULD NOT GO HERE\n");
			// printf("afd :%i\n",active->fd);
			int client_number;
			client_number = get_data_al(p->users, active);
			FD_CLR(active->fd,&p->read_set);
			Close(active->fd);
			remove_index_al(usernames,client_number);
			remove_index_al(p->users,client_number);
			update_max(p);
			//printf("nafd \n");
		}
		//Free(active);
		//printf("WHEN ITS HERE!!\n");
		Pthread_exit(0);
		//printf("Something is wrong!!!\n");
		return NULL;
	//}

}

bool verifyLogin(int connfd){

	char buf[MAXLINE];
	rio_t rio;
	Rio_readinitb(&rio,connfd);
	int read_size;
	char buf2[MAXLINE];
	bool welcome =false;

	/*Main loop to communicate with the client*/
	while((read_size =Rio_readlineb(&rio,buf,MAXLINE))!=0){

		/*For the second newLine char*/
		Rio_readlineb(&rio,buf2,MAXLINE);
		/*Recieve Welcome*/
		if(strcmp(buf,ALOLA)==0 && !welcome){
			/*Send the Response Back*/
			Rio_writen(connfd,CORRECT,sizeof(CORRECT));
			welcome =true;
		}
		else if(welcome && strncmp(buf,IAM,4)==0){
			char *username = &buf[4];
			size_t exists = get_data_al(usernames,username);

			if(exists != UINT_MAX){
				Rio_writen(connfd,ETAKEN,sizeof(ETAKEN));
				Rio_writen(connfd,username,sizeof(username));
				Close(connfd);
				return false;
			}

			insert_al(usernames,username);
			Rio_writen(connfd,MAI,sizeof(MAI));
			Rio_writen(connfd,username,sizeof(username));

			/*For ading the user to the pool*/
			client *newClient = malloc(sizeof(client));
			newClient->rio = rio;
			newClient->fd =connfd;
			newClient->lock =false;
			newClient->username = malloc(sizeof(username));
			strcpy(newClient->username,username);

			insert_al(p->users, newClient);
			FD_SET(connfd,&(p->read_set));
			if(connfd > p->maxfd)
				p->maxfd = connfd;
			p->maxi++;
			//printf("LOL\n");
			return true;
		}
		else{
			/*Not Specified*/
		}
	}
	Close(connfd);
	return false;
}

void init_pool(pool *p){

	p = malloc(sizeof(pool));
	p->maxi =0;
	p->maxfd =0;
	FD_ZERO(&p->read_set);
	FD_SET(STDIN_FILENO,&p->read_set);
	p->users= new_al(sizeof(client));
}

bool valid_fd(int fd){
	 return fcntl(fd, F_GETFD) != -1 && errno != EBADF;
}

void update_max(pool *p){

	if(p->users->length==0){
		p->maxfd = 0;
		return;
	}
	client * update = get_index_al(p->users,0);
	p->maxfd = update->fd;

	for (int i = 0; i < p->users->length; i++)
	{
		update = get_index_al(p->users,0);
		if(p->maxfd < update->fd)
			p->maxfd = update->fd;
	}
}

void handler(int sig){

}
