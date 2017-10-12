#include <QApplication>
#include "mainwindow.h"
#include <zlib.h>
#include <QIcon>
#include <unistd.h>
#include <iostream>
#include <string>
#include "tgroup.h"

#ifdef CPPLINDA
#include <linda/server.hpp>
LINDA::TUPLESPACE *linda_server;
#else
#include "sp_server.h"
#endif

#if 0
void *add_test(void *ignore){
  std::string b("aiuekaaa");
  sleep(20);
  add_tgid(b);
  return NULL;
}
#endif

int main(int argc,char *argv[]){
  QApplication app(argc,argv);
  int ret;

  if(argc != 3 && 0){
    std::cerr<<"usage : "<<argv[0]<<" port#1 port#2\n";
    exit(EXIT_FAILURE);
  }
  
#ifdef CPPLINDA
  linda_server = new LINDA::TUPLESPACE(12345,  // communication port
				       12346); // multicast     port
  linda_server->init();
  printf("LINDA server started\n");
#else
  long v_arg[2] = {argc,(long)argv};
  pthread_t spreader_tid;
  pthread_create(&spreader_tid,NULL,spreader_start,v_arg);
#endif
  std::string b("aiueo");
  std::string a("kaaiueo");


  //  pthread_t t;
  //  pthread_create(&t,NULL,add_test,NULL);
  
  std::string c;
  std::cout << b+a;
  
  int retid;
#if 0
  retid = get_first_tgid(&c);
  std::cout << "retid: " << retid << " " << c << "\n";
  printf("tgid_num = %d\n",get_tgid_num());
  add_tgid(b);
    retid = get_first_tgid(&c);
  std::cout << "retid: " << retid << " " << c << "\n";
  printf("tgid_num = %d\n",get_tgid_num());
  rm_tgid(a);
    retid = get_first_tgid(&c);
  std::cout << "retid: " << retid << " " << c << "\n";
  printf("tgid_num = %d\n",get_tgid_num());
  
  if(is_tgid(a))
    printf("exist !!!\n");
  else
    printf("not exist !!!\n");
#endif
  
  
  

  MainWindow w;
  w.setWindowIcon(QIcon(":./resources/icon256.png"));
  //w.initGlEw();
  w.show();
  // OpenGL context is created.
  // Then We can call glewInit() (especially for Windows)

  ret = app.exec();

#ifdef CPPLINDA
  linda_server->stop();
  delete linda_server;
#else
  
#endif

  return ret;
}
