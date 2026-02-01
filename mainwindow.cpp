#include "mainwindow.h"
#include <QLabel>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
  QWidget *centralWidget = new QWidget(this);
  QLabel *label = new QLabel(centralWidget);
  label->setText("Let's get some stuff done!");

  setCentralWidget(centralWidget);
}

MainWindow::~MainWindow() {}
