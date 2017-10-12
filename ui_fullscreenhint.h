/********************************************************************************
** Form generated from reading UI file 'fullscreenhint.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FULLSCREENHINT_H
#define UI_FULLSCREENHINT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>

QT_BEGIN_NAMESPACE

class Ui_fullscreen
{
public:
    QDialogButtonBox *buttonBox;
    QLabel *label;

    void setupUi(QDialog *fullscreen)
    {
        if (fullscreen->objectName().isEmpty())
            fullscreen->setObjectName(QString::fromUtf8("fullscreen"));
        fullscreen->resize(354, 175);
        buttonBox = new QDialogButtonBox(fullscreen);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(110, 130, 231, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        label = new QLabel(fullscreen);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(40, 30, 281, 71));
        QFont font;
        font.setPointSize(11);
        label->setFont(font);
        label->setScaledContents(false);
        label->setAlignment(Qt::AlignCenter);

        retranslateUi(fullscreen);
        QObject::connect(buttonBox, SIGNAL(accepted()), fullscreen, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), fullscreen, SLOT(reject()));

        QMetaObject::connectSlotsByName(fullscreen);
    } // setupUi

    void retranslateUi(QDialog *fullscreen)
    {
        fullscreen->setWindowTitle(QApplication::translate("fullscreen", "Entering fullscreen mode", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("fullscreen", "Press 'Esc' or 'Ctrl + F'\n"
"to exit Fullscreen mode.", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class fullscreen: public Ui_fullscreen {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FULLSCREENHINT_H
