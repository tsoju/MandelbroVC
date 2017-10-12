#ifndef QOPENGL_H
#define QOPENGL_H
#include "render.h"
#include "lupe.h"
#include <QtGui/QMouseEvent>
#include <QtOpenGL/QGLWidget>

#include <QTimerEvent>

#define RENDER_LAYER 3

class GLWidget : public QGLWidget{
  Q_OBJECT

  GLWidget();
  ~GLWidget();
 public:
  explicit GLWidget(QWidget *parent=0);
  virtual void initializeGL();
  void initializeGL_post();
  virtual void resizeGL(int width,int height);
  virtual void paintGL();
  class render *get_rend();
  void test();
  void flip_render_handle(class render *rend);
  void create_next_layer();
  void flip_next_layer();

  void update_pressed_xy();
  void fullscreen(int width,int height);
  void exit_fullscreen();
  bool is_fullscreen();

  void go_to(QString *_x,QString *_y);
  void get_goto_pos(QString **_x,QString **_y);

 private:
  void mouseMoveEvent(QMouseEvent *pEvent);
  void mousePressEvent(QMouseEvent *pEvent);
  void mouseReleaseEvent(QMouseEvent *pEvent);
  void mouseDoubleClickEvent(QMouseEvent *pEvent);

  void wheelEvent(QWheelEvent *e);
  

  void justASingleClick();
  QMouseEvent *storedMouseEventWhenClicked;
  
  void leaveEvent(QEvent *pEvent);
  void enterEvent(QEvent *pEvent);
  
  void setPressedPosition();
  int pressed_x;
  int pressed_y;
  int isPressed;
  
  void get_mv_length(int x,int y,int *mv_x,int *mv_y);
  void get_pos_from_center(int x,int y,int *_x,int *_y);
    
  void delete_prev_layer();

  int get_next_ruse();
  int get_prev_ruse();

  QWidget *my_parent;

 private:
  class render *rend[RENDER_LAYER];
  class lupe *rh;
  int ruse;

  QBasicTimer *timer_judg_dclick;
  int timer_judg_dclick_id;
  int dbclick_in_ms;
  bool flg_fullscreen;

  QString p_cent_crd_sx;
  QString p_cent_crd_sy;

  int sz_tget(int nvw,int nvh,int scw,int sch){
    for(int i=0;;i++)
      if( ((nvw << i) >= scw) &&
	  ((nvh << i) >= sch) )
	return i;
  };
  int mul_cnt;
  
 protected:
  void timerEvent(QTimerEvent *e);
  QWidget *centralWidget;
  QWidget *parent_of_central_widget;
  
 public:
  class lupe *get_lh();

  //  bool eventFilter(QObject *watched,QEvent *event);
  
};

#endif
