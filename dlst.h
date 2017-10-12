#ifndef DLST_H
#define DLST_H

#include <pthread.h>

struct dlst{
  int num;
  pthread_mutex_t lstlock;
  pthread_cond_t notempty;
  struct entry *head;
  struct entry **tail;
};

typedef unsigned char byte;
union _data{
  byte b_data[1];
  int i_data[1];
  char str[1];
};

struct entry{
  struct entry *next;
  int  data_size;
  union _data data;
};

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
  struct dlst *dlst_init(void);
  void dlst_del(struct dlst *lst);
  int dlst_enQ(struct dlst *list,int size,void *data);
  struct entry *dlst_deQ(struct dlst *lst);
  void dlst_free_entry(struct entry *e);
  
  void *dlst_getdata_void(struct entry *e);
  byte *dlst_getdata_byte(struct entry *e);
  int  *dlst_getdata_int (struct entry *e);
  char *dlst_getdata_str (struct entry *e);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
