#include <QEvent>
#include <QKeyEvent>
#include <QDesktopWidget>
#include "mainwindow.h"
#include "ui_uimain.h"
#include <stdio.h>
#include <unistd.h>
#include "version_window.h"
#include "ui_fullscreenhint.h"
#include "fullscreenhint.h"
#include "version.h"

void MainWindow::initGlEw(){
  rend->render_init_glew();
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),ui(new Ui::MainWindow){
  printf("MainWindow This is %p\n",this);
  ui->setupUi(this);

  worker = new class calc;
  
  rend = ui->widget->get_rend();
  printf("got rend is %p\n", rend);
  rscrl = ui->scrollArea;

  glwidget = ui->widget;
  printf("widget is %p\n",glwidget);
  
  rend->register_mainwindow(this);
  rend->initial_wndset();

  set_lupex();
  
  ui->progressBar->setValue(0);
  //  ui->statusbar->hide();
  ui->statusbar->showMessage("Hello YO");

  // ui->tableView->addItem("aaa");
  // ui->tableView->addItem("bb");
  // connect(&http,SIGNAL(done(bool)),this,SLOT(httpDone(bool)));
  qnet = new QNetworkAccessManager(this);
  connect(qnet,SIGNAL(finished(QNetworkReply*)),this,SLOT(replyFinished(QNetworkReply*)));
}

#include <iostream>
#include <string>
using namespace std;
using std::string;
void MainWindow::replyFinished(QNetworkReply *reply){
  std::string mjtk("MAJOR_VERSION="),mntk("MINOR_VERSION="),rvtk("REVISION=");
  string ver;
  int mj,mn,rv;
  int ver3;
  
  QByteArray data = reply->readAll();
  std::string str(data);
  size_t if_start;
  size_t if_end;

  if_start = str.find(mjtk);
  if(if_start == string::npos){
    fprintf(stderr,"err check version\n");
  }
  if_start += mjtk.length();
  if_end = str.find("\n",if_start);
  ver = str.substr(if_start,if_end-if_start);
  mj = atoi(ver.c_str());

  if_start = str.find(mntk);
  if(if_start == string::npos){
    fprintf(stderr,"err check version\n");
  }
  if_start += mntk.length();
  if_end = str.find("\n",if_start);
  ver = str.substr(if_start,if_end-if_start);
  mn = atoi(ver.c_str());

  if_start = str.find(rvtk);
  if(if_start == string::npos){
    fprintf(stderr,"err check version\n");
  }
  if_start += rvtk.length();
  if_end = str.find("\n",if_start);
  ver = str.substr(if_start,if_end-if_start);
  rv = atoi(ver.c_str());

  ver3 = mj * 100 + mn * 10 + rv;

  if(ver3 > MAJOR_VERSION*100 + MINOR_VERSION*10 + REVISION)
    printf("There is update!\n");
  else
    printf("Your Version is up to date\n");
}

MainWindow::~MainWindow(){
  delete ui;
  delete qnet;
  delete worker;
}

void MainWindow::check_scale_show(int state){
  if(state == Qt::Unchecked){
    ui->widget->get_lh()->set_scale_hide();
  }else if(state == Qt::Checked){
    ui->widget->get_lh()->set_scale_visible();
  }
  ui->widget->updateGL();
}

void MainWindow::check_for_update(){
  QNetworkRequest req(QUrl("http://www.softlab.cs.tsukuba.ac.jp/~soju/ver_check/mandelbro_vc_newest_ver.txt"));
  qnet->get(req);
#if 0
  file.setName("index.html");
  if(!file.open(IO_WriteOnly)){
    QMessageBox::warning(this,tr("HTTP Get"),
			 tr("Cannot write file %1\n%2.")
			 .arg(file.name())
			 .arg(file.errorString()));
    return;
  }
  http.setHost("www.softlab.cs.tsukuba.ac.jp");
  http.get("/index.html",&file);
  http.closeConnection();
#endif
}

void MainWindow::fullscreen(){
  if(glwidget->is_fullscreen())
    return;

  fullscreenhint *fscr = new fullscreenhint;
  
  //fscr->setupUi((QDialog *)this);
  if( ! fscr->exec()){
    return;
  }
  //  fscr->show();

  //  QDesktopWidget::screenGeometry();
  QDesktopWidget dscreen;
  QRect rc = dscreen.screenGeometry(glwidget);
  //  printf("pri is %d\n",dscreen.primaryScreen());
  //  printf("screen count is %d\n",dscreen.screenCount());
  printf("\t\t fscreen width: %d\n",rc.width());
  printf("\t\t fscreen height: %d\n",rc.height());

  //rc = dscreen.screenGeometry();
#if 0
  rc = dscreen.availableGeometry();
  printf("\t\t fscreen12 width: %d\n",rc.width());
  printf("\t\t fscreen12 height: %d\n",rc.height());
#endif
  
#if 0
  printf("\t window width: %d\n",size().width());
  printf("\t window height: %d\n",size().height());
  storeWindowSize(size().width(),size().height());
#endif

  QRect rc_g = geometry();
  struct window_geometry gmt;
  rc_g.getRect(&gmt.x,&gmt.y,&gmt.width,&gmt.height);
  storeWindowGeometry(&gmt);
    
  //printf("\t\t %d %d %d %d\n",x,y,g_width,g_height);
  glwidget->fullscreen(rc.width(),rc.height());
}

void MainWindow::storeWindowGeometry(struct window_geometry *gmt){
  memcpy(&saved_geometry,gmt,sizeof(struct window_geometry));
}

void MainWindow::storeWindowSize(int width,int height){
  stored_w_size_width = width;
  stored_w_size_height = height;
}

void MainWindow::restoreWindowSize(){
  //  setFixedSize(stored_w_size_width,stored_w_size_height);
  setGeometry(saved_geometry.x,saved_geometry.y,
	      saved_geometry.width,saved_geometry.height);
}

void MainWindow::redraw(){
  printf("redraw is called \n");
  glwidget->get_rend()->redraw();
}

void MainWindow::go_to(){
  QString s_x,s_y;
  
  s_x = ui->lineEdit  ->text();
  s_y = ui->lineEdit_2->text();

  glwidget->go_to(&s_x,&s_y);
}

void MainWindow::show_version_dialog(){
  VersionDialog dlg(this);
  dlg.exec();
#if 0
  Ui_Version uiv;
  uiv.setupUi(&uiv);
  uiv.setFixedSize(700,400);
  uiv.exec();
#endif
}

void MainWindow::set_mag(double mag){
  QString str;
  str.sprintf("%13.10f",mag);
  ui->lineEdit_4->setText(str);
}

void MainWindow::set_lupex(){
  QString str;
  str.sprintf("%2d x", glwidget->get_lh()->get_x());
  ui->lineEdit_3->setText(str);
}

void MainWindow::set_center_crd(double x,double y){
  QString str_x, str_y;
  str_x.sprintf("%13.10f",x);
  str_y.sprintf("%13.10f",y);
  ui->lineEdit->setText(str_x);
  ui->lineEdit_2->setText(str_y);
}
void MainWindow::set_center_crd(int which,double crd){
  QString str;

  str.sprintf("%13.10f",crd);
  if(which == LINEEDIT_CRD_X)
    ui->lineEdit->setText(str);
  else
    ui->lineEdit_2->setText(str);
}

void MainWindow::button_zoom(){
  printf("zoom pressed\n");

  rend->change_render_mode(RENDER_MODE_ZOOM);
  ui->widget->updateGL();

#if 0
  for(i=0;i<100;i++){
    rend->magnify();
  
    ui->widget->paintGL();
    //    ui->widget->update();
    ui->widget->updateGL();
    usleep(17000);
  }
#endif
}

void MainWindow::button_wide(){
  rend->change_render_mode(RENDER_MODE_PAN);
  ui->widget->updateGL();
}


void MainWindow::button_left(){
  rend->set_direction(RENDER_DIR_LEFT);
  
  rend->change_render_mode(RENDER_MODE_MV);
}

void MainWindow::button_right(){
  rend->set_direction(RENDER_DIR_RIGHT);

  rend->change_render_mode(RENDER_MODE_MV);
}

void MainWindow::button_up(){
  rend->set_direction(RENDER_DIR_UP);

  rend->change_render_mode(RENDER_MODE_MV);
}

void MainWindow::button_down(){
  rend->set_direction(RENDER_DIR_DOWN);

  rend->change_render_mode(RENDER_MODE_MV);
}

void MainWindow::button_left_step(){
  rend->set_direction(RENDER_DIR_LEFT);
  rend->move_camera_step();
}

void MainWindow::button_right_step(){
  rend->set_direction(RENDER_DIR_RIGHT);
  rend->move_camera_step();
}

void MainWindow::button_up_step(){
  rend->set_direction(RENDER_DIR_UP);
  rend->move_camera_step();
}

void MainWindow::button_down_step(){
  rend->set_direction(RENDER_DIR_DOWN);
  rend->move_camera_step();
}

void MainWindow::button_lupex_down(){
  glwidget->get_lh()->down_ocx();
  set_lupex();
}

void MainWindow::button_lupex_up(){
  glwidget->get_lh()->up_ocx();
  set_lupex();
}

void MainWindow::update_rend(){
  rend = ui->widget->get_rend();
  rend->register_mainwindow(this);
}

class render *MainWindow::get_rend(){
  return rend;
}
class GLWidget *MainWindow::get_widget(){
  return glwidget;
}

void MainWindow::changeEvent(QEvent *e){
  if(e->type() == QEvent::WindowStateChange){
    // QWindowStateChangeEvent *event = static_cast<QWindowStateChangeEvent *>(e);
    if(this->windowState() == Qt::WindowMaximized)
      if(!glwidget->is_fullscreen()){
	rscrl->adopt_lupe_size();
      }
  }
}

class RendScroll *MainWindow::get_rscrl(){
  return rscrl;
}

void MainWindow::hideMenuBar(){
  menuBar()->hide();
}

void MainWindow::showMenuBar(){
  menuBar()->show();
}

void MainWindow::show_status_msg(char *s){
  ui->statusbar->showMessage(s);
}

void MainWindow::keyPressEvent(QKeyEvent *e){
  Qt::Key key = static_cast<Qt::Key>(e->key());

  if(key == Qt::Key_unknown)
    return;

  switch(key){
  case Qt::Key_F:
    if( ! (e->modifiers() & Qt::ControlModifier))
      break;
  case Qt::Key_Escape:
    if(glwidget->is_fullscreen())
      glwidget->exit_fullscreen();
    break;

  case Qt::Key_F11:
    if( ! glwidget->is_fullscreen())
      fullscreen();
    else
      glwidget->exit_fullscreen();
    break;
	
  default:;    
  }
}

void MainWindow::clean_close(){
  printf("closed \n");
  close();
}
