struct entry {
  struct entry *next;
  void (*func)(void *);
  void *arg;
};

struct list {
  pthread_cond_t notempty;
  struct entry *head;
  struct entry **tail;
};

struct list *list_init(void);
int list_enqueue(struct list *list,void (*func)(void *),void *arg);
struct entry *list_dequeue(struct list *list);
