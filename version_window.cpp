#include <QtGui>
#include "version_window.h"
#include "version.h"

VersionDialog::VersionDialog(QWidget *parent) : QDialog(parent){
  printf("parent is %p\n",parent);
  this->setWindowTitle(tr("About Mandelbro VC"));
  this->setFixedSize(480,260);

  layout = new QVBoxLayout;
  layout_ok = new QHBoxLayout();
  layout_2h = new QHBoxLayout();
  tab = new QTabBar;
  
  QString ver_str;
  
  ver_str.sprintf("Ver %d.%d.%d",MAJOR_VERSION,MINOR_VERSION,REVISION);
  
  label1 = new QLabel(tr("<center><h1>Mandelbro VC</h1></center>"));
  label2 = new QLabel(tr("<center><h2>")+ver_str+tr("</h2></center>"));
  label3 = new QLabel(tr("<center><h3>")+QString("2017-")+tr("</h3></center>"));

  label10 = new QLabel(tr("<h2>")+QString("Programed by:")+tr("</h2>"));
  label11 = new QLabel(tr("<center><h3>")+QString("Soju Tanaka @ Softlab")+tr("</h3></center>"));
  label12 = new QLabel(tr("<center><address>")+QString("&lt;soju@softlab.cs.tsukuba.ac.jp&gt;")+tr("</address></center>"));
  label10->hide();
  label11->hide();
  label12->hide();
  
  okButton = new QPushButton(tr("OK"));

  spacer_v1 = new QSpacerItem(0,24,QSizePolicy::Minimum,QSizePolicy::Minimum);
  spacer_v2 = new QSpacerItem(0,8,QSizePolicy::Minimum,QSizePolicy::Minimum);
  spacer_v3 = new QSpacerItem(0,260,QSizePolicy::Maximum,QSizePolicy::Maximum);

  spacer_h = new QSpacerItem(360,0,QSizePolicy::Minimum,QSizePolicy::Minimum);
  spacer_h2 = new QSpacerItem(64,0,QSizePolicy::Minimum,QSizePolicy::Minimum);
  
  connect(okButton,SIGNAL(clicked()),this,SLOT(close()));

  connect(tab,SIGNAL(currentChanged(int)),this,SLOT(tab_changed(int)));

  tab->addTab(tr("Version"));
  tab->addTab(tr("Author"));
  
  layout->addWidget(tab);

  layout->addItem(spacer_v1);
  layout->addWidget(label1);

  layout_2h->addItem(spacer_h2);
  layout_2h->addWidget(label10);

  layout->addLayout(layout_2h);

  
  layout->addItem(spacer_v2);
  layout->addWidget(label2);

  layout->addWidget(label11);
  layout->addWidget(label12);

  layout->addWidget(label3);
  layout->addItem(spacer_v3);

  layout_ok->addItem(spacer_h);
  layout_ok->addWidget(okButton);

  layout->addLayout(layout_ok);

  setLayout(layout);
  show();
}

VersionDialog::~VersionDialog(){
}

void VersionDialog::tab_changed(int index){
  if(index == 0){
    label10->hide();
    label11->hide();
    label12->hide();
    label1->show();
    label2->show();
    label3->show();
  }else{
    label1->hide();
    label2->hide();
    label3->hide();
    label10->show();
    label11->show();
    label12->show();
    //layout->takeAt(0);
    //layout->takeAt(1);
  }
}
