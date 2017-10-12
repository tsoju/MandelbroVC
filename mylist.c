#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include "mylist.h"

pthread_mutex_t lst_lock = PTHREAD_MUTEX_INITIALIZER;

struct list *
list_init(void)
{
  struct list *list;

  list = malloc(sizeof *list);
  if (list == NULL)
    return (NULL);
  pthread_cond_init(&list->notempty,NULL);
  list->head = NULL;
  list->tail = &list->head;
  return (list);
}

int
list_enqueue(struct list *list,void (*func)(void *),void *arg)
{
  struct entry *e;

  e = malloc(sizeof *e);
  if (e == NULL)
    return (1);
  e->next = NULL;
  e->func = func;
  e->arg = arg;
  pthread_mutex_lock(&lst_lock);
  *list->tail = e;
  list->tail = &e->next;
  pthread_cond_signal(&list->notempty);
  pthread_mutex_unlock(&lst_lock);
  return (0);
}

struct entry *
list_dequeue(struct list *list)
{
  struct entry *e;

  pthread_mutex_lock(&lst_lock);
  while(list->head == NULL)
    pthread_cond_wait(&list->notempty,&lst_lock);
  e = list->head;
  list->head = e->next;
  if (list->head == NULL)
    list->tail = &list->head;
  pthread_mutex_unlock(&lst_lock);
  
  return (e);
}
