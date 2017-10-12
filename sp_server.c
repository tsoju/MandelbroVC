#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>

#include "logutil.h"
#include "mylist.h"
#include "dbgout.h"

#define DEFAULT_SERVER_PORT 10000
#ifdef SOMAXCONN
#define LISTEN_BACKLOG SOMAXCONN
#else
#define LISTEN_BACKLOG 5
#endif

char *program_name = "sp-server";

int
open_accepting_socket(int port)
{
  struct sockaddr_in self_addr;
  socklen_t self_addr_size;
  int sock, sockopt;

  memset(&self_addr, 0, sizeof(self_addr));
  self_addr.sin_family = AF_INET;
  self_addr.sin_addr.s_addr = INADDR_ANY;
  self_addr.sin_port = htons(port);
  self_addr_size = sizeof(self_addr);
  sock = socket(PF_INET, SOCK_STREAM, 0);
  if (sock < 0)
    logutil_fatal("accepting socket: %d", errno);
  sockopt = 1;
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
		 &sockopt, sizeof(sockopt)) == -1)
    logutil_warning("SO_REUSEADDR: %d", errno);
  if (bind(sock, (struct sockaddr *)&self_addr, self_addr_size) < 0)
    logutil_fatal("bind accepting socket: %d", errno);
  if (listen(sock, LISTEN_BACKLOG) < 0)
    logutil_fatal("listen: %d", errno);
  return (sock);
}

void
usage(void)
{
  fprintf(stderr, "Usage: %s [option]\n", program_name);
  fprintf(stderr, "option:\n");
  fprintf(stderr, "\t-d\t\t\t\t... debug mode\n");
  fprintf(stderr, "\t-p <port>\n");
  exit(1);
}

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
int hup = 0;
sigset_t hupset;
void *handle_hup(void *arg){
  int sig, err;

  (void)arg;

  err = sigwait(&hupset,&sig);
  if( (err) ||
      ((sig != SIGINT) && (sig != SIGTERM)) )
    abort();
  pthread_mutex_lock(&m);
  hup = 1;
  pthread_mutex_unlock(&m);
  return NULL;
}

#define BUFFERSIZE 1024
void server_thread(void *com){
  char request[BUFFERSIZE];
  char msg2[] = "bye\n";
  int size;
  int oldtype;

  while((size = read(*(int *)com,request,BUFFERSIZE)) != 0){
    dbgf_net("%s",request);
    write(1,request,size);
    
    pthread_mutex_lock(&m);
    if(hup){
      pthread_mutex_unlock(&m);
      write(*(int *)com, msg2, sizeof(msg2));
      close(*(int *)com);
      free(com);
      return;
    }
    pthread_mutex_unlock(&m);
  }
  
  dbgf_net("disconnected\n");
  
  pthread_setcancelstate(PTHREAD_CANCEL_DISABLE,&oldtype);
  
  close(*(int *)com);
  free(com);
}

void main_loop(struct list *list,int sock){
  int com;
  //pthread_t tid;
  int *p;
  
  while(1){
    dbgf_net("[%d] accepting incoming connections (fd==%d) ...\n", getpid(), sock);
    if ((com = accept(sock, 0, 0)) < 0) {
      perror("accept");
      close(sock);
      exit(-1);
    }
    p = malloc(sizeof(int));
    if(p == NULL){
      perror("malloc");
      close(com);
      close(sock);
      exit(-1);
    }
    *p = com;
    list_enqueue(list, server_thread, p);
#if 0
    pthread_create(&tid, NULL, server_thread, p);
#endif
  }
  
  close(sock);
}

void
cleanup(void *e){
  if(*(struct entry **)e != NULL){
    close(*(int *)(*(struct entry **)e)->arg);
    free((*(struct entry **)e)->arg);
    free(*(struct entry **)e);
  }
}

void *
get_request_and_do(void *list){
  struct entry *e=NULL;
  int oldtype;
  
  if(list == NULL)
    return NULL;

  pthread_cleanup_push(cleanup,&e);
  
  while(1){
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,&oldtype);
    dbgf_net("deque ready..\n");
    e = list_dequeue((struct list *)list);
    dbgf_net("dequed!\n");
    dbgf_net("thread_id: %u dequed\n",(unsigned int)pthread_self());

    e->func(e->arg);
    free(e);
    e = NULL;
  }
  
  pthread_cleanup_pop(0);
  
  return 0;
}

void *
cancel_thread_in_few_sec(void *tid){
  sleep(30);
  dbgf_net("Cancelling thread_id: %u..\n",((unsigned int *)tid)[0]);
  pthread_cancel(((pthread_t *)tid)[0]);
  return NULL;
}

// int
// main(int argc, char **argv)
void *
spreader_start(void *arg)
{
  int argc = (int)((long *)arg)[0];
  char **argv = (char **)((long *)arg)[1];
  
  char *port_number = NULL;
  int ch, sock, server_port = DEFAULT_SERVER_PORT;
  int debug_mode = 0;
  pthread_t tid[3];
  //pthread_t tid_d;
  int i;
  struct list *list;

  while ((ch = getopt(argc, argv, "dp:")) != -1) {
    switch (ch) {
    case 'd':
      debug_mode = 1;
      break;
    case 'p':
      port_number = optarg;
      break;
    case '?':
    default:
      usage();
    }
  }
  argc -= optind;
  argv += optind;

  list = list_init();
  if(list == NULL)
    return NULL;

  if (port_number != NULL)
    server_port = strtol(port_number, NULL, 0);

  /* server_portでlistenし，socket descriptorをsockに代入 */
  sock = open_accepting_socket(server_port);

  if (!debug_mode) {
    logutil_syslog_open(program_name, LOG_PID, LOG_LOCAL0);
    //daemon(0, 0);
  }

  pthread_t t;
  sigemptyset(&hupset);
  sigaddset(&hupset,SIGINT);
  sigaddset(&hupset,SIGTERM);
  
  pthread_sigmask(SIG_BLOCK,&hupset,NULL);
  pthread_create(&t,NULL,handle_hup,NULL);

  for(i=0; i<3; i++){
    pthread_create(&tid[i],NULL,get_request_and_do,list);
  }
  //pthread_create(&tid_d,NULL,cancel_thread_in_few_sec,tid);

  main_loop(list, sock);

  /*NOTREACHED*/
  return NULL;
}
