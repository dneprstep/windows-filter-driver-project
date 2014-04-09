/********************************************************************************
** Form generated from reading UI file 'gui.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GUI_H
#define UI_GUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GUIClass
{
public:
    QWidget *centralWidget;
    QComboBox *comboBox;
    QPushButton *Filter_Apply;
    QLabel *label;
    QListWidget *listWidget;
    QPushButton *Show_Selected_File;
    QPushButton *Hide_ALL;
    QPushButton *Reset_ALL_filters;
    QLineEdit *lineEdit;
    QPushButton *Filter_Add;
    QPushButton *pushButton;

    void setupUi(QMainWindow *GUIClass)
    {
        if (GUIClass->objectName().isEmpty())
            GUIClass->setObjectName(QStringLiteral("GUIClass"));
        GUIClass->resize(356, 583);
        centralWidget = new QWidget(GUIClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        comboBox = new QComboBox(centralWidget);
        comboBox->setObjectName(QStringLiteral("comboBox"));
        comboBox->setGeometry(QRect(20, 460, 171, 22));
        Filter_Apply = new QPushButton(centralWidget);
        Filter_Apply->setObjectName(QStringLiteral("Filter_Apply"));
        Filter_Apply->setGeometry(QRect(200, 460, 75, 23));
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 440, 46, 13));
        listWidget = new QListWidget(centralWidget);
        listWidget->setObjectName(QStringLiteral("listWidget"));
        listWidget->setGeometry(QRect(20, 20, 256, 361));
        Show_Selected_File = new QPushButton(centralWidget);
        Show_Selected_File->setObjectName(QStringLiteral("Show_Selected_File"));
        Show_Selected_File->setGeometry(QRect(24, 390, 251, 41));
        QFont font;
        font.setPointSize(14);
        Show_Selected_File->setFont(font);
        Show_Selected_File->setCheckable(true);
        Show_Selected_File->setChecked(false);
        Hide_ALL = new QPushButton(centralWidget);
        Hide_ALL->setObjectName(QStringLiteral("Hide_ALL"));
        Hide_ALL->setGeometry(QRect(280, 150, 75, 51));
        QFont font1;
        font1.setPointSize(10);
        font1.setBold(true);
        font1.setWeight(75);
        Hide_ALL->setFont(font1);
        Reset_ALL_filters = new QPushButton(centralWidget);
        Reset_ALL_filters->setObjectName(QStringLiteral("Reset_ALL_filters"));
        Reset_ALL_filters->setGeometry(QRect(280, 210, 71, 51));
        QFont font2;
        font2.setPointSize(12);
        font2.setBold(true);
        font2.setWeight(75);
        Reset_ALL_filters->setFont(font2);
        lineEdit = new QLineEdit(centralWidget);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));
        lineEdit->setGeometry(QRect(20, 510, 251, 20));
        Filter_Add = new QPushButton(centralWidget);
        Filter_Add->setObjectName(QStringLiteral("Filter_Add"));
        Filter_Add->setGeometry(QRect(120, 540, 75, 23));
        pushButton = new QPushButton(centralWidget);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(280, 500, 71, 41));
        GUIClass->setCentralWidget(centralWidget);

        retranslateUi(GUIClass);

        QMetaObject::connectSlotsByName(GUIClass);
    } // setupUi

    void retranslateUi(QMainWindow *GUIClass)
    {
        GUIClass->setWindowTitle(QApplication::translate("GUIClass", "GUI", 0));
        Filter_Apply->setText(QApplication::translate("GUIClass", "Apply filter", 0));
        label->setText(QApplication::translate("GUIClass", "<html><head/><body><p><span style=\" font-size:10pt;\">Filters</span></p></body></html>", 0));
#ifndef QT_NO_TOOLTIP
        listWidget->setToolTip(QApplication::translate("GUIClass", "<html><head/><body><p>BLocked files</p></body></html>", 0));
#endif // QT_NO_TOOLTIP
        Show_Selected_File->setText(QApplication::translate("GUIClass", "Show file", 0));
        Hide_ALL->setText(QApplication::translate("GUIClass", "Hide ALL", 0));
        Reset_ALL_filters->setText(QApplication::translate("GUIClass", "RESET \n"
"ALL", 0));
        Filter_Add->setText(QApplication::translate("GUIClass", "Add filter", 0));
        pushButton->setText(QApplication::translate("GUIClass", "Add filter...", 0));
    } // retranslateUi

};

namespace Ui {
    class GUIClass: public Ui_GUIClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GUI_H
