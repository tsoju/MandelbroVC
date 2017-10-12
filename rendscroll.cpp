#include <stdio.h>
#include <QResizeEvent>
#include  <QScrollBar>
#include "rendscroll.h"
#include "glwidget.h"

RendScroll::RendScroll(QWidget *parent):QScrollArea(parent){
  printf("RendScroll This is %p\n",this);
  //  setGeometry(QRect(1024,80,1024,1024));
  vbar = this->verticalScrollBar();
  hbar = this->horizontalScrollBar();
  //  vbar->setValue(100);
  //  vbar->setValue(vbar->value() + 100);
  //  this->setWidgetResizable(true);
  this->setBackgroundRole(QPalette::Dark);
  //  this->setMinimumHeight(500);

  mwnd = (MainWindow *)parent->parentWidget();
  
  //  wgt = ((MainWindow *)parent->parentWidget())->get_widget();
}


void RendScroll::init_called_post_widget_set(){
  //setGeometry(QRect(1024,80,1024,1024));
  //vbar = this->verticalScrollBar();
  //vbar->setValue(100);
  //vbar->setValue(vbar->value() + 100);
}

RendScroll::~RendScroll(){
}

void RendScroll::cotton_resize(int width,int height){

  int mul_cnt = sz_tget(VIEWPORT_WIDTH,VIEWPORT_HEIGHT,
			width,height);
  
  VIEWPORT_WIDTH <<= mul_cnt;
  VIEWPORT_HEIGHT <<= mul_cnt;
  
  mwnd->get_widget()->setMinimumSize(VIEWPORT_WIDTH,VIEWPORT_HEIGHT);
  mwnd->get_widget()->setMaximumSize(VIEWPORT_WIDTH,VIEWPORT_HEIGHT);
  widget()->setMinimumSize(VIEWPORT_WIDTH,VIEWPORT_HEIGHT);

  adopt_lupe_size(width,height);

#if 0
  printf("\t\t\toverd %d x %d\n",VIEWPORT_WIDTH,VIEWPORT_HEIGHT);
  }else if( (rzEvent->size().width()  <= VIEWPORT_WIDTH >>1) &&
	    (rzEvent->size().height() <= VIEWPORT_HEIGHT>>1) ){

    printf("\t\t\t KSAPKDSAKD\n");
  printf("V_WIDTH: %d\n",VIEWPORT_WIDTH);
  printf("V_HEIGHT: %d\n",VIEWPORT_HEIGHT);

    VIEWPORT_WIDTH  >>= 1;
    VIEWPORT_HEIGHT >>= 1;

  printf("V_WIDTH: %d\n",VIEWPORT_WIDTH);
  printf("V_HEIGHT: %d\n",VIEWPORT_HEIGHT);

    printf("VWIDTH:%d VHEIGHT:%d\n",VIEWPORT_WIDTH,VIEWPORT_HEIGHT);
    mwnd->get_widget()->setMinimumSize(VIEWPORT_WIDTH,VIEWPORT_HEIGHT);
    mwnd->get_widget()->setMaximumSize(VIEWPORT_WIDTH,VIEWPORT_HEIGHT);
    widget()->setMinimumSize(VIEWPORT_WIDTH,VIEWPORT_HEIGHT);
    printf("\t\t CCCCCCCC\n");
  }

#define MIN_W (INITIAL_VPORT_WIDTH  >> 1)
#define MIN_H (INITIAL_VPORT_HEIGHT >> 1)
  widget()->setMinimumWidth(rzEvent->size().width()*(VIEWPORT_WIDTH/MIN_W));
  widget()->setMinimumHeight(rzEvent->size().height()*(VIEWPORT_HEIGHT/MIN_H));
  
  hbar->setValue(rzEvent->size().width()  * (my_pow(2,sc_tget(MIN_W*2,VIEWPORT_WIDTH ))-1)/2);
  vbar->setValue(rzEvent->size().height() * (my_pow(2,sc_tget(MIN_H*2,VIEWPORT_HEIGHT))-1)/2);

  adopt_lupe_size();
#endif
}

void RendScroll::resizeEvent(QResizeEvent *rzEvent){
  printf("rxEvent.width: %d\n",rzEvent->size().width());
  printf("rxEvent.height: %d\n",rzEvent->size().height());
  //  static int c;
  
  //  ((GLWidget *)widget())->setGeometry(QRect(1024,80,1024,1024));

  printf("Old Size %d x %d\n",rzEvent->oldSize().width(),rzEvent->oldSize().height());
  printf("New Size %d x %d\n",rzEvent->size().width(),rzEvent->size().height());
  
  //scrollContentsBy(100,-300);
  //scroll(-30,100);
  //  ensureVisible(700,800,200,300);
  // ensureWidgetVisible(widget(),50,100);

  //this->resize(rzEvent->size().width(),rzEvent->size().height());
#if 0
  if(c++%2)
    this->resize(1000,512);
  else
    this->resize(1000,256);
#endif

  if( (rzEvent->size().width()  > VIEWPORT_WIDTH) ||
      (rzEvent->size().height() > VIEWPORT_HEIGHT) ){
    VIEWPORT_WIDTH  <<= 1;
    VIEWPORT_HEIGHT <<= 1;
    mwnd->get_widget()->setMinimumSize(VIEWPORT_WIDTH,VIEWPORT_HEIGHT);
    mwnd->get_widget()->setMaximumSize(VIEWPORT_WIDTH,VIEWPORT_HEIGHT);
    widget()->setMinimumSize(VIEWPORT_WIDTH,VIEWPORT_HEIGHT);
    printf("\t\t\toverd %d x %d\n",VIEWPORT_WIDTH,VIEWPORT_HEIGHT);
    //    mwnd->get_widget()->get_rend()->mag_mag(0.5);
  }else if( (rzEvent->size().width()  <= VIEWPORT_WIDTH >>1) &&
	    (rzEvent->size().height() <= VIEWPORT_HEIGHT>>1) ){
    VIEWPORT_WIDTH  >>= 1;
    VIEWPORT_HEIGHT >>= 1;

    mwnd->get_widget()->setMinimumSize(VIEWPORT_WIDTH,VIEWPORT_HEIGHT);
    mwnd->get_widget()->setMaximumSize(VIEWPORT_WIDTH,VIEWPORT_HEIGHT);
    widget()->setMinimumSize(VIEWPORT_WIDTH,VIEWPORT_HEIGHT);
    printf("\t\t CCCCCCCC\n");
    //    mwnd->get_widget()->get_rend()->mag_mag(2.0);
  }

#define MIN_W (INITIAL_VPORT_WIDTH  >> 1)
#define MIN_H (INITIAL_VPORT_HEIGHT >> 1)
  // printf("%p da\n",widget());
  //((GLWidget *)widget())->resizeGL(100,200);
  widget()->setMinimumWidth(rzEvent->size().width()*(VIEWPORT_WIDTH/MIN_W));
  widget()->setMinimumHeight(rzEvent->size().height()*(VIEWPORT_HEIGHT/MIN_H));
  
  hbar->setValue(rzEvent->size().width()  * (my_pow(2,sc_tget(MIN_W*2,VIEWPORT_WIDTH ))-1)/2);
  vbar->setValue(rzEvent->size().height() * (my_pow(2,sc_tget(MIN_H*2,VIEWPORT_HEIGHT))-1)/2);
//  hbar->setValue(rzEvent->size().width()*3/2);
  //  printf("A: %d\n",rzEvent->size().width());
  //  printf("B: %d\n",(rzEvent->size().width() * (my_pow(2,(VIEWPORT_WIDTH /(MIN_W*2)))-1))/2);
  //  printf("C: %d\n",my_pow(2,(VIEWPORT_WIDTH /(MIN_W*2)))-1);
  //  printf("D: %d\n",sc_tget(MIN_W*2,VIEWPORT_WIDTH));
  
  //hbar->setValue( rzEvent->size().width()/2);
  //  vbar->setValue(rzEvent->size().height() * (my_pow(2,(VIEWPORT_HEIGHT/(MIN_H*2)))-1)/2);
  //  printf("AAAAAAAAAAAAAAAA: %d\n",(my_pow(2,(VIEWPORT_WIDTH /(MIN_W*2)))-1)/2);
  //  printf("AAAAAAAAAAAAAAAA: %d\n",(my_pow(2,(VIEWPORT_HEIGHT /(MIN_H*2)))-1)/2);
  //vbar->setValue(rzEvent->size().height()/2);
  
  //  widget()->setMinimumWidth(rzEvent->size().width()*4);
  //widget()->setMinimumHeight(rzEvent->size().height()*4);
  //hbar->setValue(rzEvent->size().width()*3/2);
  //vbar->setValue(rzEvent->size().height()*3/2);

  adopt_lupe_size();
}



#define SUNKEN_LINE_WIDTH 1
void RendScroll::size_without_sunken(int *width,int *height){
  *width = size().width()-SUNKEN_LINE_WIDTH * 2;
  *height = size().height()-SUNKEN_LINE_WIDTH * 2;
}

void RendScroll::adopt_lupe_size(int width,int height){
  mwnd->get_widget()->get_lh()->set_size(width,height);
  printf("\t\t width %d , height:%d\n",width,height);
}

void RendScroll::adopt_lupe_size(){
  int width,height;
  
  size_without_sunken(&width,&height);

  printf("@@@@@@@@@@@@ adopted\n");
  printf("width:%d, height:%d\n",width,height);

  //printf("lupe: %p\n", wgt->get_lh());
  // printf("glwidget: %p\n", wgt);
  //printf("parent : %p\n",parentWidget());
  //printf("widget : %p\n",((MainWindow *)parentWidget())->get_widget());
  mwnd->get_widget()->get_lh()->set_size(width,height);
  //wgt->get_lh()->set_size(width,height);
  
  //Qt EventFilterthis->resize(1024,1024);  
}
