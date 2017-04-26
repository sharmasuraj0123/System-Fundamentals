#include "server_helper.h"


void *login_thread(void * vargp){

Pthread_detach(pthread_self());
while(1){
	int *connfd = remove_index_al(login , 0);
	if(connfd!=NULL)
		if(verifyLogin(*connfd)){
			//communicate(*connfd);
			client *testClient = get_index_al(p->users,0);
			printf("%s\n",testClient->username);
			printf("%d\n",testClient->fd);
		}
}
}

void* communication_thread(void* vargp){

	Pthread_detach(pthread_self());

	while(1){

		int i,n;
		client *active = malloc(sizeof(client));
		char buf[MAXLINE];

		if(p->maxi!=0){
			p->ready_set = p->read_set;
			p->nready = Select(p->maxfd+1,&p->ready_set,NULL,NULL,NULL);
			printf("%s\n","Hesdllosd");
			for(i=0; i<p->maxi;i++){
				printf("%s\n","Hellosd");
				active = get_index_al(p->users,i);
				if(FD_ISSET(active->fd,&p->ready_set)){
					printf("%s\n","Hello");
					p->nready--;

				if((n =Rio_readlineb(&active->rio,buf,MAXLINE))!=0){
					Rio_writen(active->fd,buf,n);
				}
				}
			}

		}

	}
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
		read_size = Rio_readlineb(&rio,buf2,MAXLINE);
		/*Recieve Welcome*/
		if(strcmp(buf,ALOLA)==0 && !welcome){
			/*Send the Response Back*/
			Rio_writen(connfd,CORRECT,sizeof(CORRECT));
			welcome =true;
		}
		else if(welcome && strncmp(buf,IAM,3)==0){
			char *username = &buf[3];
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
			newClient->username = username;

			insert_al(p->users, newClient);
			FD_SET(connfd,&(p->read_set));
			if(connfd > p->maxfd)
				p->maxfd = connfd;
			p->maxi++;

			printf("LOL\n");
			return true;
		}
		else{
			/*Not Specified*/
		}
	}
	Close(connfd);
	return false;
}

void communicate(int connfd){

	char buf[MAXLINE];
	rio_t rio;
	Rio_readinitb(&rio,connfd);
	int read_size;
	char buf2[MAXLINE];

	/*Main loop to communicate with the client*/
	while((read_size =Rio_readlineb(&rio,buf,MAXLINE))!=0){

		/*For the second newLine char*/
		read_size = Rio_readlineb(&rio,buf2,MAXLINE);
		if(strncmp(buf,MSG,3)==0){

		}
		else if(strcmp(buf,WHO)==0){

		}
		else{
			/*Not specified*/
		}
	}

}

void init_pool(pool *p){

	p = malloc(sizeof(pool));
	p->maxi =0;
	p->maxfd =0;
	FD_ZERO(&p->read_set);
	p->users= new_al(sizeof(client));
}




