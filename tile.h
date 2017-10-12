#ifndef TILE_H
#define TILE_H
#include <stdio.h>

struct area_coord{
  double x_s; /* -2.0 - 2.0 etc */
  double y_s;
  double x_e;
  double y_e;
  double w;
  double w_h;
  double h;
  double h_h;
};

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <QList>
#include <QMutex>
#include <sys/time.h>
#include "render.h"
#include "mandelbro.h"

#define TEXTURE_SEL_0 0
#define TEXTURE_SEL_1 1

struct mandel_task{
  int x_start; /* 0 - 1023 etc */
  int x_end;
  int y_start;
  int y_end;
};

struct subtile_pos{
  int x;
  int y;
};

struct task_result{
};

class render;

// Status
#define TILE_NONE 0  // TILE 生成直後
#define TILE_INITIALIZED 1 // 初期Texture 用意済
#define TILE_CALCULATED 2 // 計算完了
#define TILE_PREPARED 4 // TILE 完成済み
#define TILE_SUBTILE_UPDATED 8 // SUBTILE 更新有

#define LAREA_LE 3
class tile{
 public:
  static int init();
  static GLubyte *larea_texdt[LAREA_LE];
  static GLuint larea_tex[LAREA_LE];
  static void mk_larea_data(GLubyte *dt,int le);

  int find_le(int width,int height);

  tile();
  tile(render *parent,struct area_coord *crd,int w,int h,int x,int y);
  //tile(struct area_coord *crd,int w,int h);
  ~tile();
  int calc_me();
  int make_texture_data();
  
  int make_texture_loading();
  int make_texture_update();
  int make_texture();
  int make_texture_subtile_update(int x,int y);
  
  GLubyte *get_texture_data();
  GLuint get_texture();
  int get_x_idx();
  int get_y_idx();

  int prepare_tile_in_new_thread();
  static void *launch_thread(void *arg){
    //reinterpret_cast<tile *>(arg)->prepare_thread_local();
    ((tile *)arg)->prepare_thread_local();
    //printf("tile thread is ended !@!!!!\n");
    pthread_exit(NULL);
  }
  int prepare_thread_local();

  void show();
  int get_status();

  void query_subtile_update(int x,int y);
  void notify_subtile_update();
  void update_subtile();
  
 private:
  int status;
  
 private:
  struct area_coord area;
  class mandelbro *tile_man;
  int width;
  int height;
  int x_idx;
  int y_idx;
  GLubyte *texture_data;
  GLuint texture[2];
  int tex_sel;
  struct area_coord *get_area_crd();
  render *parent_render;
  
  //QList<struct mandel_task> list;
  QList<struct subtile_pos> q_update_pos;
  QMutex mutex_uppos_lst;

  int update_cnt_sum;
  // struct timeval s,e;
};

#endif
