#ifndef TASK_SPAWNER_H
#define TASK_SPAWNER_H
#include "mandelbro.h"
#include "tile.h"

struct mandel_task_info{
  int pos;
  double x_s;
  double y_s;
  double x_e;
  double y_e;
  int w;
  int h;
};

struct tsp_task_info{
  struct area_coord crd;
  int dw;
  int dh;
  int div_h;
  int div_v;
  char uuid_s[64];
};

class task_spawner : public QObject{
  Q_OBJECT

 public:
  task_spawner();
  task_spawner(struct tsp_task_info *ti);
  ~task_spawner();

 public slots:
   void process();

 signals:
   void finished();
   void error(QString err);

 private:
   struct tsp_task_info ti;
};

#endif
