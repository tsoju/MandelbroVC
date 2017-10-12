#ifndef FULLSCREENHINT_H_
#define FULLSCREENHINT_H_

#include <QObject>
#include <QDialog>
#include "ui_fullscreenhint.h"

namespace Ui{
  class fullscreen;
}

class fullscreenhint: public QDialog, public Ui::fullscreen{
  Q_OBJECT
 public:
  explicit fullscreenhint();
  ~fullscreenhint();
};

#endif
