#ifndef RENDSCROLL_H
#define RENDSCROLL_H

#include <QScrollArea>
#include "mainwindow.h"
#include "glwidget.h"

class RendScroll : public QScrollArea{
  Q_OBJECT

  RendScroll();
  ~RendScroll();

 public:
  void resizeEvent(QResizeEvent *);
  void cotton_resize(int width,int height);

  
  
 public:
  explicit RendScroll(QWidget *parent=0);
  void init_called_post_widget_set();
  void adopt_lupe_size();
  void adopt_lupe_size(int width,int height);
  
 private:
    void size_without_sunken(int *width,int *height);
    int my_pow(int x,int n){
      int ret = 1;
      for(int i=0;i<n;i++){
	ret *= x;
      }
      return ret;
    };
    int sc_tget(int low,int max){
      for(int i=1;;i++)
	if(low << i > max) return i;
    };
    int sz_tget(int nvw,int nvh,int scw,int sch){
      for(int i=0;;i++)
	if( ((nvw << i) >= scw) &&
	    ((nvh << i) >= sch) )
	  return i;
    };
  
 private:
  QScrollBar *vbar;
  QScrollBar *hbar;
  class GLWidget *wgt;
  class MainWindow *mwnd;
    
};

#endif
