// #include <iostream>
// #include <stdio.h>
//#include <QEvent>
#include <QHBoxLayout>
#include "glwidget.h"
// #include "mandelbro.h"
// #include "tile.h"
// using namespace std;


class render *GLWidget::get_rend(){
  return rend[ruse];
}

void GLWidget::test(){
  printf("test is called\n");
}

GLWidget::GLWidget(QWidget *parent):QGLWidget(parent){
  printf("GLWidget this is %p\n",this);
  printf("GLWidget> parent is %p\n",parent);
  // printf("GLWidget Parent is %p\n",parent);
  // printf("GLWidget Grand Parent is %p\n",parent->parentWidget());
  timer_judg_dclick = new class QBasicTimer;
  /* It is better to get double click interval time(ms)
     by environment specific API. */
  dbclick_in_ms = 400;
  storedMouseEventWhenClicked = (QMouseEvent *)malloc(sizeof(QMouseEvent));

  printf("GLwidget create lupe\n");
  rh = new class lupe(this);
  printf("Addr is %p GLwidget create lupe\n",rh);

  tile::init();
  
  for(int i=0;i<RENDER_LAYER;i++)
    rend[i] = NULL;
  
  rend[0] = new class render(this,rh);
  printf("first rend is %p\n", rend[0]);
  ruse = 0;

  isPressed = 0;
  flg_fullscreen = false;
}

GLWidget::~GLWidget(){
  int i;
  for(i=0;i<RENDER_LAYER;i++)
    if(rend[i] != NULL)
      delete rend[i];

  delete rh;
  delete timer_judg_dclick;

  free(storedMouseEventWhenClicked);
}


void GLWidget::get_mv_length(int x,int y,int *mv_x,int *mv_y){
  *mv_x = x - pressed_x;
  *mv_y = pressed_y - y;
}

void GLWidget::get_pos_from_center(int x,int y,int *_x,int *_y){
  *_x =  x - (VIEWPORT_WIDTH  >> 1);
  *_y = -y + (VIEWPORT_HEIGHT >> 1);
}

#define IGNORE_MV_LEN 2
void GLWidget::mouseMoveEvent(QMouseEvent *pEvent){
  int mv_x,mv_y;

  if(rend[ruse]->get_render_mode() == RENDER_MODE_MV_POS){
    if(rh->isShow())
      rh->update_lupe_pos(pEvent->x(),pEvent->y());
    return;
  }
  
  if(isPressed){
    get_mv_length(pEvent->x(),pEvent->y(),&mv_x,&mv_y);
    rend[ruse]->update_mv_length(mv_x,mv_y);

    if(abs(mv_x) + abs(mv_y) < IGNORE_MV_LEN){
      timer_judg_dclick->stop();
      timer_judg_dclick->start(dbclick_in_ms,this);
      timer_judg_dclick_id = timer_judg_dclick->timerId();
    }else{
      timer_judg_dclick->stop();
    }
    
    rend[ruse]->mv_camera_from_drag(mv_x,mv_y);
  }else{
    int pos_x,pos_y;
    get_pos_from_center(pEvent->x(),pEvent->y(),&pos_x,&pos_y);
    // printf("cursor @ %d %d\n",pos_x,pos_y);
    rend[ruse]->update_cursor_pos(pos_x,pos_y);
  }

  if(rh->isShow()){
    rh->update_lupe_pos(pEvent->x(),pEvent->y());
    this->updateGL();
  }
  // rend[ruse]->d_print_camera_pos();
}

void GLWidget::mousePressEvent(QMouseEvent *pEvent){
  int rend_md_flg = rend[ruse]->get_render_mode_flg();
  if(pEvent->button() == Qt::RightButton){
    if(rend_md_flg & FLG_RENDER_MODE_MVZM)
      return;
    rh->cycle_ocx();
    rend[ruse]->get_mainwindow()->set_lupex();
    return;
  }else if(pEvent->button() != Qt::LeftButton){
    return;
  }
      
  setCursor(Qt::ClosedHandCursor);

  if( rend_md_flg & (FLG_RENDER_MODE_ZOOM |
		     FLG_RENDER_MODE_MVZM) ){

    return;
  }

  rh->set_hide();
    
  printf("mouth is pressed x: %d, y: %d\n",pEvent->x(),pEvent->y());
  pressed_x = pEvent->x();
  pressed_y = pEvent->y();
  rend[ruse]->store_camera_pos();
  rend[ruse]->drag_unit_update(); /* dolphin ? */

  //rh->update_lupe_pos(pEvent->x(),pEvent->y());
  
  isPressed = 1;
  this->updateGL();

  timer_judg_dclick->start(dbclick_in_ms,this);
  timer_judg_dclick_id = timer_judg_dclick->timerId();
  memcpy(storedMouseEventWhenClicked,pEvent,sizeof(QMouseEvent));
}

void GLWidget::update_pressed_xy(){
  QPoint ptgl = mapFromGlobal(QCursor::pos());

  pressed_x = ptgl.x();
  pressed_y = ptgl.y();
}

class lupe *GLWidget::get_lh(){
  printf("get_lh() give %p\n",rh);
  return rh;
}

void GLWidget::mouseReleaseEvent(QMouseEvent *pEvent){
  setCursor(Qt::OpenHandCursor);
  printf("mouth is released x: %d, y: %d\n",pEvent->x(),pEvent->y());

  int pos_x,pos_y;
  get_pos_from_center(pEvent->x(),pEvent->y(),&pos_x,&pos_y);
  rend[ruse]->update_cursor_pos(pos_x,pos_y);

  //rend[ruse]->get_mainwindow()->show_status_msg(NULL);

#if 0
  if( (rend[ruse]->get_render_mode() == RENDER_MODE_MVZM) ||
      (rend[ruse]->get_render_mode() == RENDER_MODE_ZOOM) ||
      (rend[ruse]->get_render_mode() == RENDER_MODE_SZOOM) ){
    return;
  }
#endif

  if( rend[ruse]->get_render_mode_flg() &
      (FLG_RENDER_MODE_MVZM  
       | FLG_RENDER_MODE_ZOOM  
       // | FLG_RENDER_MODE_SZOOM
       // | FLG_RENDER_MODE_SPAN
       )){
    // printf("\t\t\t ignored\n");
    return;
  }
   
  //  updateGL();

  if(rend[ruse]->get_render_mode() != RENDER_MODE_MV_POS)
    rend[ruse]->change_render_mode(RENDER_MODE_NORMAL);

  rh->set_visible();
  rh->update_lupe_pos(pEvent->x(),pEvent->y());
  
  isPressed = 0;

  this->updateGL();
}

void GLWidget::justASingleClick(){
  printf("########## mouse is Single-Clicked\n");
  //printf("x:%d y:%d\n",storedMouseEventWhenClicked->x(),	 storedMouseEventWhenClicked->y());
  rend[ruse]->store_mv_pos(storedMouseEventWhenClicked->x(),storedMouseEventWhenClicked->y());
  rend[ruse]->change_render_mode(RENDER_MODE_MV_POS);
}

void GLWidget::mouseDoubleClickEvent(QMouseEvent *pEvent){
  if(pEvent->button() == Qt::RightButton){
    mousePressEvent(pEvent);
    return;
  }
  if(pEvent->button() != Qt::LeftButton){
    return;
  }
  
  timer_judg_dclick->stop();
  printf("########## mouse is Double-Clicked\n");
  rend[ruse]->store_mv_pos(pEvent->x(),pEvent->y());
  printf("##########%d %d \n",pEvent->x(),pEvent->y());
  rend[ruse]->change_render_mode(RENDER_MODE_MVZM);
  rh->set_disable();
}

void GLWidget::wheelEvent(QWheelEvent *e){
#if 0
  // Q_UNUSED(e);
  //  rend[ruse]->get_mainwindow()->setWindowFlags(0);
  //  rend[ruse]->get_mainwindow()->resize( QApplication::desktop()->size());
  
  rend[ruse]->get_mainwindow()->setFocusPolicy(Qt::StrongFocus );
  rend[ruse]->get_mainwindow()->setAttribute(Qt::WA_QuitOnClose, true);
  
  rend[ruse]->get_mainwindow()->show();
  //  rend[ruse]->get_mainwindow()->setFocus();
  //  rend[ruse]->get_mainwindow()->showFullScreen();
#endif
  
  int step;
  step = (int)((double)e->delta()/120.0);

  int rend_status = rend[ruse]->get_render_mode_flg();
  if(step > 0){
    if(rend_status & FLG_RENDER_MODE_SZOOM)
      rend[ruse]->szoom_increment();
    else
      rend[ruse]->change_render_mode(RENDER_MODE_SZOOM);
  }else if(step < 0){
    if(rend_status & FLG_RENDER_MODE_SPAN)
      rend[ruse]->span_increment();
    else
      rend[ruse]->change_render_mode(RENDER_MODE_SPAN);
  }

}

#if 0
bool GLWidget::eventFilter(QObject *watched,QEvent *event){
  //  if(watched == this && event->type() == QEvent::MouseMove){
  if(watched == this){
    printf("\t\t\tX%d XXX\n",event->type());
    return false;
  }
  printf("\t\t\taskdojsadoajsdoi\n");
  return false;
}
#endif

void GLWidget::leaveEvent(QEvent *pEvent){
  Q_UNUSED(pEvent);
  rh->set_hide();
  rend[ruse]->get_mainwindow()->show_status_msg(NULL);
  
  this->updateGL();
}
void GLWidget::enterEvent(QEvent *pEvent){
  Q_UNUSED(pEvent);
  rh->set_visible();
  this->updateGL();
}

void GLWidget::timerEvent(QTimerEvent *e){
  //Q_UNUSED(e);
  if(e->timerId() == timer_judg_dclick_id){
    printf("TIMER CALLED \n");
    timer_judg_dclick->stop();
    justASingleClick();
  }else{
    this->updateGL();
  }
}


GLuint texhandle[16];
GLfloat texCoord[]={
  1.0,0,1.0,1.0,0,0,0,1.0,
};


void GLWidget::initializeGL(){
  glClearColor(0.2,0.9,0.9,1.0);
  rh->init();
}
void GLWidget::initializeGL_post(){
}

void GLWidget::paintGL(){
  glClear(GL_COLOR_BUFFER_BIT);

  rend[ruse]->draw();
  
  glFlush();
  glFinish();
}

void GLWidget::resizeGL(int width,int height){
  glViewport(0,0,width,height);
  glLoadIdentity();
  // glOrtho(0.0,1.0,0.0,1.0,-1.0,1.0);

  printf("resized width:%d, height %d\n",width,height);

  // showFullScreen();
  // scroll(0,-100);
  
}

int GLWidget::get_next_ruse(){
  if(ruse < RENDER_LAYER-1)
    return ruse + 1;
  else
    return 0;
}

int GLWidget::get_prev_ruse(){
  if(ruse == 0)
    return RENDER_LAYER - 1;
  else
    return ruse - 1;
}

void GLWidget::create_next_layer(){
  struct render_info ri;
  rend[ruse]->get_render_info(&ri);

  ruse = get_next_ruse();
  if(rend[ruse] != NULL)
    delete rend[ruse];
  rend[ruse] = new class render(this,&ri,rh);
  rend[get_prev_ruse()]->cp_gl_object(rend[ruse]);
}

void GLWidget::flip_next_layer(){
  rend[get_prev_ruse()]->get_mainwindow()->update_rend();
  
}

void GLWidget::delete_prev_layer(){
  delete rend[ruse-1];
  rend[ruse-1] = NULL;
}

void GLWidget::fullscreen(int width,int height){
  flg_fullscreen = true;

  int mul_cnt = sz_tget(VIEWPORT_WIDTH,VIEWPORT_HEIGHT,width,height);

  printf("mul cnt is %d\n",mul_cnt);

  rend[ruse]->get_mainwindow()->get_rscrl()->cotton_resize(width,height);
  
  
#if 0
  
  printf("mvlen is %d\n",-(((VIEWPORT_WIDTH << mul_cnt) - width) >> 1));

  
  printf("parent is %p\n", parentWidget());
  
  //    printf("index: %d\n",my_parent->indexOf(this));
  //my_parent->showMaximized();
  //    installEventFilter(this);
  // showMaximized();
  //    showFullScreen();
#endif

  my_parent = parentWidget();
  setParent(NULL);    
  centralWidget = rend[ruse]->get_mainwindow()->centralWidget();
  
  parent_of_central_widget = centralWidget->parentWidget();
  centralWidget->setParent(NULL);
  rend[ruse]->get_mainwindow()->setCentralWidget(this);
  //  show();
  // my_parent->showMaximized();
  rend[ruse]->get_mainwindow()->showMaximized();
  rend[ruse]->get_mainwindow()->hideMenuBar();
  rend[ruse]->get_mainwindow()->showFullScreen();

  printf("VHEIGHT:%d, heifht %d\n",VIEWPORT_HEIGHT,height);

  QRect rc;
  rc.setWidth(width);
  rc.setHeight(height);
  rc.setLeft(-(VIEWPORT_WIDTH-width)/2);
  rc.setTop(-(VIEWPORT_HEIGHT-height)/2);

  rend[ruse]->get_mainwindow()->setGeometry(rc);

#if 0
  rend[ruse]->get_mainwindow()->setGeometry(
					    //-(VIEWPORT_WIDTH-width)/2,
					    0,
					    //1900,
					    1920-1600,
					    //-(VIEWPORT_WIDTH-width)/2,
					    //-(VIEWPORT_HEIGHT-height)/2,
					    width +(VIEWPORT_WIDTH -width) /2,
					    //height+(VIEWPORT_HEIGHT-height)/1
					    height
					    );
#endif
  //  rend[ruse]->get_mainwindow()->move(300,600);
    
  //    showMaximized();
  //   showFullScreen();
  //  rend[ruse]->get_mainwindow()->move(200,300);
  // my_parent->move(200,500);
  
}

void GLWidget::go_to(QString *_x,QString *_y){
  p_cent_crd_sx = *_x;
  p_cent_crd_sy = *_y;
  rend[ruse]->change_render_mode(RENDER_MODE_GOTO);
}

void GLWidget::get_goto_pos(QString **_x,QString **_y){
  *_x = &p_cent_crd_sx;
  *_y = &p_cent_crd_sy;
}

void GLWidget::exit_fullscreen(){
  QSize sz;
  printf("V_WIDTH: %d\n",VIEWPORT_WIDTH);
  printf("V_HEIGHT: %d\n",VIEWPORT_HEIGHT);
  sz.setWidth(VIEWPORT_WIDTH >> mul_cnt);
  sz.setHeight(VIEWPORT_HEIGHT >> mul_cnt);
  printf("V_WIDTH: %d\n",VIEWPORT_WIDTH);
  printf("V_HEIGHT: %d\n",VIEWPORT_HEIGHT);
  
  QResizeEvent rsz_ev(sz,sz);
  for(int i=0;i<mul_cnt;i++)
    //    rend[ruse]->get_mainwindow()->get_rscrl()->cotton_resize(&rsz_ev);
  
  printf("revert\n");
  printf("centralWidget is %p\n",centralWidget);
  //    rend[ruse]->get_mainwindow()->setCentralWidget(centralWidget);
  printf("\t\t\t AAA %d\n",((QLayout *)my_parent)->indexOf(this));
  
  QHBoxLayout *layout = my_parent->findChild<QHBoxLayout *>(QString("horizontalLayout"));
  printf("layout is %p\n",layout);
  layout->addWidget(this);
  
  
  setParent(my_parent);
  //    centralWidget->setParent(NULL);
  rend[ruse]->get_mainwindow()->setCentralWidget(centralWidget);
  setMinimumSize(QSize(1024,1024));
  setMaximumSize(QSize(1024,1024));
  //    centralWidget->setParent(centralWidget);
  rend[ruse]->get_mainwindow()->showNormal();
  rend[ruse]->get_mainwindow()->showMenuBar();
  showNormal();
  //my_parent->setCentralWidget(this);
  rend[ruse]->get_mainwindow()->restoreWindowSize();
  flg_fullscreen = false;
}

bool GLWidget::is_fullscreen(){
  return flg_fullscreen;
}
