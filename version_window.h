#ifndef VERSION_WINDOW_H_
#define VERSION_WINDOW_H_

#include <QtGui>
#include <QDialog>
#include <QApplication>
#include <QTabBar>
#include <QTextEdit>
#include <QSpacerItem>

class QLabel;
class QPushButton;

class VersionDialog : public QDialog{
  Q_OBJECT
 public:
  VersionDialog(QWidget *parent = 0);
  ~VersionDialog();
  
 private slots:
   void tab_changed(int index);
      
 private:
   QPushButton *okButton;
   
   QSpacerItem *spacer_v1;
   QSpacerItem *spacer_v2;
   QSpacerItem *spacer_v3;
   
   QSpacerItem *spacer_h;
   QSpacerItem *spacer_h2;
   
   QLabel *label1;
   QLabel *label2;
   QLabel *label3;
   QLabel *label10;
   QLabel *label11;
   QLabel *label12;

   QVBoxLayout *layout;
   QHBoxLayout *layout_ok;
   QHBoxLayout *layout_2h;
   QTabBar *tab;
};

#endif
