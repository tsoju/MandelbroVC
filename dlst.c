#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "dlst.h"

struct dlst *dlst_init(void){
  struct dlst *list;

  
  list = malloc(sizeof(struct dlst));
  if(list == NULL)
    return NULL;
  list->num = 0;
  pthread_mutex_init(&list->lstlock,NULL);
  pthread_cond_init(&list->notempty,NULL);
  list->head = NULL;
  list->tail = &list->head;
  return list;
}

void dlst_del(struct dlst *lst){
  pthread_cond_destroy(&lst->notempty);
  pthread_mutex_destroy(&lst->lstlock);
  free(lst);
}

int dlst_enQ(struct dlst *list,int size,void *data){
  struct entry *e;

  e = malloc(sizeof(struct entry) - 1 + size);
  if(e == NULL)
    return -1;

  e->next = NULL;
  e->data_size = size;
  
  memcpy(&e->data,data,size);

  pthread_mutex_lock(&list->lstlock);
    *list->tail = e;
    list->tail = &e->next;
    list->num++;
    pthread_cond_signal(&list->notempty);
  pthread_mutex_unlock(&list->lstlock);
  
  return 0;
}

struct entry *dlst_deQ(struct dlst *list){
  struct entry *e;

  pthread_mutex_lock(&list->lstlock);
    while(list->head == NULL)
      pthread_cond_wait(&list->notempty,&list->lstlock);
    e = list->head;
    list->head = e->next;
    if(list->head == NULL)
      list->tail = &list->head;
    list->num--;
  pthread_mutex_unlock(&list->lstlock);

  return e;
}

void dlst_free_entry(struct entry *e){
  free(e);
}

void *dlst_getdata_void(struct entry *e){
  return &e->data;
}
byte *dlst_getdata_byte(struct entry *e){
  return e->data.b_data;
}
int *dlst_getdata_int(struct entry *e){
  return e->data.i_data;
}
char *dlst_getdata_str (struct entry *e){
  return e->data.str;
}
