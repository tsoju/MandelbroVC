#ifndef LUPE_H
#define LUPE_H

#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include "render.h"
#include "glwidget.h"

class GLWidget;

class lupe{
 public:
  lupe();
  lupe(GLWidget *glw);
  ~lupe();

  int ocx_num();
    
  void draw_lupe();
  void draw_scale();
  
  bool isShow();
  void set_visible();
  void set_hide();
  void set_enable();
  void set_disable();

  bool isScaleShow();
  void set_scale_visible();
  void set_scale_hide();
  
  void init();
  void update_lupe_pos(int x,int y);

  int get_x();
  int get_xidx(int idx);
  int get_idx();
  
  void cycle_ocx();
  void up_ocx();
  void down_ocx();

  void set_size(int width,int height);

  void store_current_lp_pos();
  void show_stored_lupe();
  void hide_stored_lupe();
  void draw_stored_lupe();
  bool isShow_stored_lupe;
  
 private:
  GLWidget *glw;
  int x_ocular;
  unsigned int x_idx;
  
  bool flg_show;
  bool flg_scale_show;
  bool enable;
  
  GLubyte *mk_lupe_window_rgba(int sides,int waku);
  GLuint texture[2];
  GLubyte *texture_data[2];

  GLfloat center_x;
  GLfloat center_y;

  GLfloat stored_lppos_x[2];
  GLfloat stored_lppos_y[2];
  
 public:
  GLfloat ratio_x;
  GLfloat ratio_y;
};

#endif
