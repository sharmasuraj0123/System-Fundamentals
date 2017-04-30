#include "debug.h"
#include "arraylist.h"
#include "foreach.h"
#include "server.h"


int main(int argc, char *argv[]){

	int listenfd, *connfd;
	socklen_t clientlen;
	struct sockaddr clientaddr;
	pthread_t tid;

	int n = 1;
	/*Initializing the arraylists*/
	usernames = new_al(sizeof(char *));
	login = new_al(sizeof(connfd));
	printf("%d\n",getpid());
	//users = new_al(sizeof(client));

	//init_pool(p);

	p = malloc(sizeof(pool));
	p->maxi =0;
	p->maxfd =0;
	FD_ZERO(&p->read_set);
	FD_SET(STDIN_FILENO,&p->read_set);
	p->users= new_al(sizeof(client));
	stdin_lock = false;

	listenfd = Open_listenfd(PORT);

	/*Creating login threads based on n*/
	for(int i =0; i< n; i++)
		Pthread_create(&tid,NULL,login_thread,NULL);

	/*creating a communication thread*/
		Pthread_create(&tid,NULL,communication_thread,NULL);

	while(1){
		clientlen = sizeof(struct sockaddr_storage);
		connfd = malloc(sizeof(int *));
		*connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
		//printf("Request from %d\n",*connfd);
		insert_al(login,connfd);
		}
}
