/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>
#include "mygl.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionQuit;
    QAction *actionCamera_Controls;
    QWidget *centralWidget;
    MyGL *mygl;
    QListWidget *vertsListWidget;
    QListWidget *halfEdgesListWidget;
    QListWidget *facesListWidget;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QDoubleSpinBox *vertPosXSpinBox;
    QDoubleSpinBox *vertPosYSpinBox;
    QDoubleSpinBox *vertPosZSpinBox;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_6;
    QLabel *label_7;
    QLabel *label_8;
    QLabel *label_9;
    QDoubleSpinBox *faceBlueSpinBox;
    QLabel *label_10;
    QDoubleSpinBox *faceGreenSpinBox;
    QDoubleSpinBox *faceRedSpinBox;
    QLabel *label_11;
    QPushButton *importOBJButton;
    QMenuBar *menuBar;
    QMenu *menuFile;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1057, 492);
        actionQuit = new QAction(MainWindow);
        actionQuit->setObjectName("actionQuit");
        actionCamera_Controls = new QAction(MainWindow);
        actionCamera_Controls->setObjectName("actionCamera_Controls");
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName("centralWidget");
        mygl = new MyGL(centralWidget);
        mygl->setObjectName("mygl");
        mygl->setGeometry(QRect(11, 11, 618, 432));
        vertsListWidget = new QListWidget(centralWidget);
        vertsListWidget->setObjectName("vertsListWidget");
        vertsListWidget->setGeometry(QRect(640, 10, 111, 261));
        halfEdgesListWidget = new QListWidget(centralWidget);
        halfEdgesListWidget->setObjectName("halfEdgesListWidget");
        halfEdgesListWidget->setGeometry(QRect(770, 10, 111, 261));
        facesListWidget = new QListWidget(centralWidget);
        facesListWidget->setObjectName("facesListWidget");
        facesListWidget->setGeometry(QRect(900, 10, 111, 261));
        label = new QLabel(centralWidget);
        label->setObjectName("label");
        label->setGeometry(QRect(640, 280, 111, 16));
        label->setAlignment(Qt::AlignmentFlag::AlignCenter);
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(770, 280, 111, 16));
        label_2->setAlignment(Qt::AlignmentFlag::AlignCenter);
        label_3 = new QLabel(centralWidget);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(900, 280, 111, 16));
        label_3->setAlignment(Qt::AlignmentFlag::AlignCenter);
        vertPosXSpinBox = new QDoubleSpinBox(centralWidget);
        vertPosXSpinBox->setObjectName("vertPosXSpinBox");
        vertPosXSpinBox->setGeometry(QRect(720, 320, 62, 22));
        vertPosXSpinBox->setMinimum(-99.989999999999995);
        vertPosYSpinBox = new QDoubleSpinBox(centralWidget);
        vertPosYSpinBox->setObjectName("vertPosYSpinBox");
        vertPosYSpinBox->setGeometry(QRect(800, 320, 62, 22));
        vertPosYSpinBox->setMinimum(-99.989999999999995);
        vertPosZSpinBox = new QDoubleSpinBox(centralWidget);
        vertPosZSpinBox->setObjectName("vertPosZSpinBox");
        vertPosZSpinBox->setGeometry(QRect(880, 320, 62, 22));
        vertPosZSpinBox->setMinimum(-99.989999999999995);
        label_4 = new QLabel(centralWidget);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(640, 320, 71, 21));
        label_5 = new QLabel(centralWidget);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(720, 340, 61, 16));
        label_5->setAlignment(Qt::AlignmentFlag::AlignCenter);
        label_6 = new QLabel(centralWidget);
        label_6->setObjectName("label_6");
        label_6->setGeometry(QRect(800, 340, 61, 16));
        label_6->setAlignment(Qt::AlignmentFlag::AlignCenter);
        label_7 = new QLabel(centralWidget);
        label_7->setObjectName("label_7");
        label_7->setGeometry(QRect(880, 340, 61, 16));
        label_7->setAlignment(Qt::AlignmentFlag::AlignCenter);
        label_8 = new QLabel(centralWidget);
        label_8->setObjectName("label_8");
        label_8->setGeometry(QRect(880, 390, 61, 16));
        label_8->setAlignment(Qt::AlignmentFlag::AlignCenter);
        label_9 = new QLabel(centralWidget);
        label_9->setObjectName("label_9");
        label_9->setGeometry(QRect(720, 390, 61, 16));
        label_9->setAlignment(Qt::AlignmentFlag::AlignCenter);
        faceBlueSpinBox = new QDoubleSpinBox(centralWidget);
        faceBlueSpinBox->setObjectName("faceBlueSpinBox");
        faceBlueSpinBox->setGeometry(QRect(880, 370, 62, 22));
        faceBlueSpinBox->setMaximum(1.000000000000000);
        faceBlueSpinBox->setSingleStep(0.050000000000000);
        label_10 = new QLabel(centralWidget);
        label_10->setObjectName("label_10");
        label_10->setGeometry(QRect(640, 370, 71, 21));
        faceGreenSpinBox = new QDoubleSpinBox(centralWidget);
        faceGreenSpinBox->setObjectName("faceGreenSpinBox");
        faceGreenSpinBox->setGeometry(QRect(800, 370, 62, 22));
        faceGreenSpinBox->setMaximum(1.000000000000000);
        faceGreenSpinBox->setSingleStep(0.050000000000000);
        faceRedSpinBox = new QDoubleSpinBox(centralWidget);
        faceRedSpinBox->setObjectName("faceRedSpinBox");
        faceRedSpinBox->setGeometry(QRect(720, 370, 62, 22));
        faceRedSpinBox->setMaximum(1.000000000000000);
        faceRedSpinBox->setSingleStep(0.050000000000000);
        label_11 = new QLabel(centralWidget);
        label_11->setObjectName("label_11");
        label_11->setGeometry(QRect(800, 390, 61, 16));
        label_11->setAlignment(Qt::AlignmentFlag::AlignCenter);
        importOBJButton = new QPushButton(centralWidget);
        importOBJButton->setObjectName("importOBJButton");
        importOBJButton->setGeometry(QRect(960, 350, 75, 24));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName("menuBar");
        menuBar->setGeometry(QRect(0, 0, 1057, 22));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName("menuFile");
        MainWindow->setMenuBar(menuBar);

        menuBar->addAction(menuFile->menuAction());
        menuFile->addAction(actionQuit);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "OpenGLDemo", nullptr));
        actionQuit->setText(QCoreApplication::translate("MainWindow", "Quit", nullptr));
#if QT_CONFIG(shortcut)
        actionQuit->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Q", nullptr));
#endif // QT_CONFIG(shortcut)
        actionCamera_Controls->setText(QCoreApplication::translate("MainWindow", "Camera Controls", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Vertices", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "Half-Edges", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "Faces", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "Vertex Position", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "X", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "Y", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "Z", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "Blue", nullptr));
        label_9->setText(QCoreApplication::translate("MainWindow", "Red", nullptr));
        label_10->setText(QCoreApplication::translate("MainWindow", "Face Color", nullptr));
        label_11->setText(QCoreApplication::translate("MainWindow", "Green", nullptr));
        importOBJButton->setText(QCoreApplication::translate("MainWindow", "Import OBJ", nullptr));
        menuFile->setTitle(QCoreApplication::translate("MainWindow", "File", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
