#include <iostream>
#include <QApplication>
#include "mainwindow.h"
#include "service.h"

int main(int argc, char *argv[]) {
  std::cout << "Starting adhd program" << std::endl;

  QApplication a(argc, argv);
  a.setStyle("fusion");

  MainWindow w;
  w.show();

  return a.exec();
}
