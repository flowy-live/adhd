#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include <QMainWindow>

class Service;
class SessionsListModel;

QT_BEGIN_NAMESPACE
QT_END_NAMESPACE

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  private slots:
  void on_selectVaultButton_clicked();

  private:
  Ui::MainWindow *ui;
  Service *m_service;
  SessionsListModel *m_listModel;
};

#endif // MAINWINDOW_H
