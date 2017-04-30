#ifndef SERVER_HELPER_H
#define SERVER_HELPER_H

#include <pthread.h>

#include "debug.h"
#include "arraylist.h"
#include "foreach.h"
#include "const.h"
#include "sys/socket.h"
#include "csapp.h"

#define ALOLA "ALOLA!\r\n"
#define IAM "IAM "
#define CORRECT "!ALOLA \r\n\r\n"
#define ETAKEN "ETAKEN "
#define MAI "MAI "
#define EOL "\r\n\r\n"
#define WHO "WHO\r\n"
#define MSG "MSG "
#define GSM "GSM "
#define OHW "OHW "
#define COLON ":"
#define SERVER "SERVER"
#define NEWLINE "\r\n"
#define SPACE " "

typedef struct{
	int fd;
	rio_t rio;
	char * username;
	bool lock;
}client;

typedef struct{
	int maxfd;
	fd_set read_set;
	fd_set ready_set;
	int nready;
	int maxi;
	arraylist_t *users;
}pool;


arraylist_t * login;
arraylist_t *usernames;
pool *p;
bool stdin_lock;

void update_max(pool *p);
/*Thread Handlers*/
void *login_thread(void * vargp);
void* communication_thread(void* vargp);
void* broadcast_thread(void* vargp);


bool verifyLogin(int connfd);
/**/
void init_pool(pool *p);
bool valid_fd(int fd);
void handler(int sig);
#endif
