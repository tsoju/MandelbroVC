#ifndef MAIN_H_
#define MAIN_H_

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include "render.h"
#include "ui_version.h"
#include "rendscroll.h"
#include "calc.h"

#define LINEEDIT_CRD_X 0
#define LINEEDIT_CRD_Y 1

namespace Ui{
  class MainWindow;
}

struct window_geometry{
  int x;
  int y;
  int width;
  int height;
};

class MainWindow: public QMainWindow{
    Q_OBJECT
 public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void set_center_crd(double x,double y);
    void set_center_crd(int which,double crd);
    void set_mag(double mag);
    void set_lupex();
    
    void update_rend();
    void initGlEw();
    class render *get_rend();
    class GLWidget *get_widget();
    void changeEvent(QEvent *e);
    class RendScroll *get_rscrl();
    void hideMenuBar();
    void showMenuBar();
    void show_status_msg(char *s);
    void restoreWindowSize();
      
 private slots:
   void button_zoom();
   void button_wide();
   void button_left();
   void button_right();
   void button_up();
   void button_down();
   void button_left_step();
   void button_right_step();
   void button_up_step();
   void button_down_step();

   void clean_close();

   void button_lupex_up();
   void button_lupex_down();

   void check_scale_show(int);
   
   void show_version_dialog();
   void check_for_update();
   void fullscreen();
   void redraw();
   void go_to();

   void replyFinished(QNetworkReply*);


 private:
    Ui::MainWindow *ui;
    class render *rend;
    class GLWidget *glwidget;
    QNetworkAccessManager *qnet;
    class RendScroll *rscrl;

    void keyPressEvent(QKeyEvent *e);
    void storeWindowSize(int width,int height);
    int stored_w_size_width;
    int stored_w_size_height;

    void storeWindowGeometry(struct window_geometry *gmt);
    struct window_geometry saved_geometry;
    class calc *worker;
    
};

class Ui_Version : public Ui_Dialog, public QDialog{};


#endif
