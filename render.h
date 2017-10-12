#ifndef RENDER_H
#define RENDER_H
#include <QBasicTimer>
#include <QTimer>
#include "tile.h"
#include "lupe.h"
#include "glwidget.h"
#include "mainwindow.h"

class tile;
class lupe;
class GLWidget;
class MainWindow;

extern int ocx[];

//#define VIEWPORT_WIDTH 1024
//#define VIEWPORT_HEIGHT 1024
#define INITIAL_VPORT_WIDTH 1024
#define INITIAL_VPORT_HEIGHT 1024

#define VIEWPORT_WIDTH render::vpt_wh
#define VIEWPORT_HEIGHT render::vpt_ht

#if 0
#define RENDER_MODE_NORMAL 0
#define RENDER_MODE_ZOOM   1
#define RENDER_MODE_PAN    2
#define RENDER_MODE_MV     3
#define RENDER_MODE_MV_POS 4
#define RENDER_MODE_MVZM   5
#define RENDER_MODE_SZOOM  6
#define RENDER_MODE_SPAN   7
#define RENDER_MODE_GOTO   8
#endif

enum RENDER_MODE{
  RENDER_MODE_NORMAL=0,
  RENDER_MODE_ZOOM,
  RENDER_MODE_PAN,
  RENDER_MODE_MV,
  RENDER_MODE_MV_POS,
  RENDER_MODE_MVZM,
  RENDER_MODE_SZOOM,
  RENDER_MODE_SPAN,
  RENDER_MODE_GOTO,
  RENDER_MODE_NUM
};

#define FLG_RENDER_MODE_NORMAL (1 << RENDER_MODE_NORMAL)
#define FLG_RENDER_MODE_ZOOM   (1 << RENDER_MODE_ZOOM)
#define FLG_RENDER_MODE_PAN    (1 << RENDER_MODE_PAN)
#define FLG_RENDER_MODE_MV     (1 << RENDER_MODE_MV)
#define FLG_RENDER_MODE_MV_POS (1 << RENDER_MODE_MV_POS)
#define FLG_RENDER_MODE_MVZM   (1 << RENDER_MODE_MVZM)
#define FLG_RENDER_MODE_SZOOM  (1 << RENDER_MODE_SZOOM)
#define FLG_RENDER_MODE_SPAN   (1 << RENDER_MODE_SPAN)
#define FLG_RENDER_MODE_GOTO   (1 << RENDER_MODE_GOTO)

#define RENDER_DIR_LEFT 0
#define RENDER_DIR_RIGHT 1
#define RENDER_DIR_UP 2
#define RENDER_DIR_DOWN 3

struct render_info{
  GLdouble cent_crd_x;
  GLdouble cent_crd_y;
  GLdouble base_mag;
  bool show_prev_layer;
};

struct scene{
  int mode;
  int isFirst;
  int isAnimation;
  int need_prepare;
  int count_max;
  int count;
  int (render::*rend)();
  int (render::*prepare)();
  int (render::*fin)();
};

class render{
 public:
  static int vpt_wh;
  static int vpt_ht;

  render();
  render(GLWidget *widget,lupe *lh);
  render(GLWidget *widget,struct render_info *ri,lupe *lh);
  void render_init_glew();
  
  ~render();
  void initial_wndset();
  int draw();
  void magnify();
  void wide();
  void change_render_mode(RENDER_MODE mode);
  int get_render_mode();
  int get_render_mode_flg();
  void set_direction(int dir);
  void mv_camera_from_drag(int x,int y);
  void get_render_info(struct render_info *ri);
  void cp_gl_object(render *to);

  //static double _f_clzoom(double x);
  double _f_init_(double x,unsigned long long max);
  double _f_clzoom(double x,unsigned long long max);
  double _f_chzoom(double x,unsigned long long max);
  
 private:
  GLWidget *glw;
  MainWindow *wnd;
  lupe *lh;

  GLdouble center_crd_x;
  GLdouble center_crd_y;
  
  tile *tl;
  tile **tl_array;
  int width;
  int height;
  GLdouble mag_x; /* 1.0 - 8.0 for display */
  GLdouble mag_y;

  GLdouble stored_mag_x;
  GLdouble stored_mag_y;
  
  GLuint fbo;
  GLuint colorBuffer;
  GLuint texture;

  enum RENDER_MODE render_mode; /* 現在のrender mode  */
  QTimer *timer;

  struct scene mtype[RENDER_MODE_NUM];

  GLdouble camera_pos_x;
  GLdouble camera_pos_y;
  GLdouble camera_pos_x_to;
  GLdouble camera_pos_y_to;
  GLdouble *camera_pos_step_array_0;
  GLdouble *camera_pos_step_array;
  GLdouble *camera_pos_step_array_goto;
  GLdouble *camera_pos_step_x;
  GLdouble *camera_pos_step_y;
  GLdouble stored_camera_pos_x;
  GLdouble stored_camera_pos_y;

  GLdouble *camera_zoom_step_array;
  GLdouble *camera_zoom_step_array_s;
  GLdouble *camera_zoom_step_x;
  GLdouble *camera_zoom_step_y;
  GLdouble camera_zoom_x_to;
  GLdouble camera_zoom_y_to;

  GLdouble *camera_mvzm_step_array_m;
  GLdouble *camera_mvzm_step_array_z;
  GLdouble **camera_mvzm_step_array_z_sum;

  GLdouble camera_zoom_diff_x;
  GLdouble camera_zoom_diff_y;

  int stored_mvpos_x;
  int stored_mvpos_y;
  GLfloat mv_pos_len_x;
  GLfloat mv_pos_len_y;

  void create_step_array_mv();
  void create_step_array_mv_goto();
  void create_step_array_mvzm();
  void create_step_array_zoom(GLdouble diff);
  void create_step_array_zoom_s();

  void init_render_mode();

  int mv_direction;
  void camera_mv_smooth();
  void camera_mv(GLdouble x,GLdouble y);
  void camera_mv_to(GLdouble x,GLdouble y);
  void camera_zoom_smooth();
  void camera_zoom_smooth_l();
  void camera_zoom(GLdouble addmag_x,GLdouble addmag_y);
  void camera_zoom_to(GLdouble x,GLdouble y);
  void camera_pan_smooth();
  void camera_mv_smooth_topos();
  void camera_mv_smooth_topos_goto();
  void camera_mv_smooth_topos_f_mag();

  int tl_num_all;
  void tl_null_fill();

  void set_render_tile_index();
  void set_render_tile_index_all();
  int set_render_tile_index_calc(GLdouble camera_pox,int *st);
  int set_render_tile_index_calc_u1(GLdouble camera_pox,int *st);
  int render_need_st_x;
  int render_need_x_num;
  int render_need_st_y;
  int render_need_y_num;
  int create_tile_in_view();
  int render_current_tile_in_view();
  int draw_tile(tile *tl);
    
  int calc_tile_index(int x_idx,int y_idx);

  void flip_to_new_render();
  void initialize_scene();


 public:
  void register_mainwindow(MainWindow *wnd);
  MainWindow *get_mainwindow();
  void update();

  int pre_normal();
  int render_normal();
  
  int pre_zoom();  
  int render_zoom();
  int fin_zoom();
  
  int pre_pan();  
  int render_pan();
  int fin_pan();
  
  int pre_mv();
  int render_mv();
  int fin_mv();

  void store_mv_pos(int x,int y);
  int pre_mvps();
  int render_mvps();
  int fin_mvps();

  int pre_mvzm();
  int render_mvzm();
  int fin_mvzm();

  int pre_szoom();
  int render_szoom();
  int fin_szoom();
  void szoom_increment();
  bool is_szoom_incremented;

  int pre_span();
  int render_span();
  int fin_span();
  void span_increment();
  bool is_span_incremented;

  int pre_goto();
  int render_goto();
  int fin_goto();
  
  void move_camera_step();

  void mag_mag(GLdouble);

  void get_mag(GLdouble *x,GLdouble *y);
  
  /* 座標 */
  double x;
  double y;
  double base_mag;

  GLdouble mv_camera_drag_unit_x,mv_camera_drag_unit_y;
  void store_camera_pos();
  void drag_unit_update();
  void get_camera_pos(GLdouble *x,GLdouble *y);

 public:
  void d_print_camera_pos();
  void update_cursor_pos(int cur_x,int cur_y);
  void update_mv_length(int mv_x,int mv_y);
  void redraw();

};

#endif
