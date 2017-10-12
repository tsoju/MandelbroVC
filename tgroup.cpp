#include <iostream>
#include <map>
#include <vector>
#include "dlst.h"
#include <pthread.h>
#include "task_spawner.h"

using namespace std;

static pthread_mutex_t vlock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t vnotempty = PTHREAD_COND_INITIALIZER;
static map<string,int> extask;
static int tgid_num;

static pthread_mutex_t vvlock = PTHREAD_MUTEX_INITIALIZER;
static map<string,struct dlst *> task_res;

static pthread_mutex_t vvvlock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t vvvnotempty = PTHREAD_COND_INITIALIZER;
static multimap<std::string,struct mandel_task_info> task_lst;

//  struct dlst *dlst_init(void);

void add_task(std::string &tname,struct mandel_task_info *ti){
  pthread_mutex_lock(&vvvlock);
    task_lst.insert(std::pair<string,struct mandel_task_info>(tname,*ti));
    pthread_cond_signal(&vvvnotempty);
  pthread_mutex_unlock(&vvvlock);
}

void get_task(std::string *tname,struct mandel_task_info *ti){
  pthread_mutex_lock(&vvvlock);
    while(task_lst.size() == 0)
      pthread_cond_wait(&vvvnotempty,&vvvlock);

    const map<string,struct mandel_task_info>::const_iterator iend = task_lst.end();
    map<string,struct mandel_task_info>::iterator i;
    i = task_lst.begin();
    if(i == iend)
      printf("fatal\n");
    else{
      *tname = i->first;
      *ti = i->second;
      task_lst.erase(i);
    }
  pthread_mutex_unlock(&vvvlock);
}

void add_tgid(string &tgid){
  pthread_mutex_lock(&vlock);
    const map<string,int>::const_iterator iend = extask.end();
    map<string,int>::const_iterator i;
    i = extask.find(tgid);
    if(i == iend){
      extask.insert(std::pair<string,int>(tgid,1));
      tgid_num++;
      pthread_cond_signal(&vnotempty);

      struct dlst *lst1 = dlst_init(); // should check null
      struct dlst *lst2 = dlst_init(); // should check null
      string sfix1("_TASK");
      string sfix2("_RSLT");
      pthread_mutex_lock(&vvlock);
        task_res.insert(std::pair<string,struct dlst *>
			(tgid+sfix1,lst1));
	task_res.insert(std::pair<string,struct dlst *>
			(tgid+sfix2,lst2));
      pthread_mutex_unlock(&vvlock);
      
    }
  pthread_mutex_unlock(&vlock);
}

struct dlst *get_task_list(string &tgid){
  struct dlst *ret = NULL;
  pthread_mutex_lock(&vlock);
    const map<string,int>::const_iterator iend = extask.end();
    map<string,int>::const_iterator i;
    
    i = extask.find(tgid);
    if(i != iend){
      string sfix("_TASK");
      map<string,struct dlst *>::const_iterator j;
      pthread_mutex_lock(&vvlock);
        j = task_res.find(tgid+sfix);
	ret = j->second;
      pthread_mutex_unlock(&vvlock);
    }
  pthread_mutex_unlock(&vlock);
  
  return ret;
}

struct dlst *get_result_list(string &tgid){
  struct dlst *ret = NULL;
  pthread_mutex_lock(&vlock);
    const map<string,int>::const_iterator iend = extask.end();
    map<string,int>::const_iterator i;
    
    i = extask.find(tgid);
    if(i != iend){
      string sfix("_RSLT");
      map<string,struct dlst *>::const_iterator j;
      pthread_mutex_lock(&vvlock);
        j = task_res.find(tgid+sfix);
	ret = j->second;
      pthread_mutex_unlock(&vvlock);
    }
  pthread_mutex_unlock(&vlock);
  
  return ret;
}

bool is_tgid(string &tgid){
  pthread_mutex_lock(&vlock);
    const map<string,int>::const_iterator iend = extask.end();
    map<string,int>::const_iterator i;

    i = extask.find(tgid);
  pthread_mutex_unlock(&vlock);
      
  if(i == iend){
    return false;
  }
  return true;
}

void rm_tgid(string &tgid){
  pthread_mutex_lock(&vlock);
    const map<string,int>::const_iterator iend = extask.end();
    map<string,int>::const_iterator i;

    i = extask.find(tgid);
    if(i != iend){
      map<string,struct dlst *>::const_iterator j;
      string sfix1("_TASK");
      string sfix2("_RSLT");
      pthread_mutex_lock(&vvlock);
        j = task_res.find(tgid+sfix1);
	dlst_del(j->second);
	j = task_res.find(tgid+sfix2);
	dlst_del(j->second);
	task_res.erase(tgid+sfix1);
	task_res.erase(tgid+sfix2);
      pthread_mutex_unlock(&vvlock);
      
      extask.erase(tgid);
      tgid_num--;
    }
  pthread_mutex_unlock(&vlock);
}

int get_tgid_num(void){
  return tgid_num;
}

int get_first_tgid(std::string *gettgid){
  int ret;

  pthread_mutex_lock(&vlock);
    while(tgid_num == 0)
      pthread_cond_wait(&vnotempty,&vlock);
    
    const map<string,int>::const_iterator iend = extask.end();
    map<string,int>::const_iterator i;
    i = extask.begin();
    if(i == iend)
      ret = -1;
    else{
      ret = 0;
      *gettgid = i->first;
    }
  pthread_mutex_unlock(&vlock);
  
  return ret;
}
