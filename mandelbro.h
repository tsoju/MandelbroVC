#ifndef MANDELBRO_H
#define MANDELBRO_H
#include <QThread>
#include "tile.h"
#include "task_spawner.h"

class tile;
class task_spawner;
class mandelbro{
 public:
  mandelbro();
  mandelbro(tile *parent,struct area_coord *crd,int w,int h,int de);
  ~mandelbro();
  int calc();
  int *get_mandel_res();
  void show();

  GLubyte *get_texture_data(int x,int y);
  
 private:
#if 0
  GLubyte ***texture_data;
#endif
  GLubyte **texture_data;
  tile *ptile;
  void update_rectangle(int x,int y,unsigned char *ud);
  void create_subtile_tex(int x,int y,unsigned char *ud);
  int div_h; /* 横方向の分割数 */
  int div_v; /* 縦方向の分割数 */
  int dw; /* 分割後横方向画素数 */
  int dh; /* 分割後縦方向画素数 */
  int w; /* 元の横方向画素数 */
  int h; /* 元の縦方向画素数 */
  double x_s_;
  double y_s_;
  double x_e_;
  double y_e_;
  int x_pix_num; /* タイルを構成するx方向画素数 */
  int y_pix_num; /* タイルを構成するy方向画素数 */
  int max_iterate; /* 発散調べるループ最大反復数 */
  double x_pix_length; /* x1画素幅長 */
  double y_pix_length; /* y1画素幅長 */
  double half_x_pix_length; /* x1画素幅の半分長 */
  double half_y_pix_length; /* y1画素幅の半分長 */
  
  int *res;
  char uuid_s[64];
};


#endif
