#ifndef CALC_H
#define CALC_H

#include <pthread.h>

#define THREAD_NUM 4

struct mandel_task_result{
  int pos;
  int data[1];
};

class calc{
 public:
  calc();
  ~calc();
  static void *worker(void *arg);
  
 private:
  pthread_t thread_handle[THREAD_NUM];
};


#endif
