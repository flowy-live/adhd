#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include <QMainWindow>

class Service;
class SessionsListModel;
class FocusOverlay;
class QTimer;

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
  void on_newSessionButton_clicked();
  void on_startSessionButton_clicked();
  void on_endSessionButton_clicked();

  // Service signal handlers
  void onActiveSessionChanged();
  void onSessionTimerTicked(int elapsedMs);

  // Active tab field editing
  void onActiveOnelinerChanged();
  void onActiveContentChanged();

  // Overlay handlers
  void onOverlayResetClicked();
  void onOverlayEndSessionClicked();

private:
  Ui::MainWindow *ui;
  Service *m_service;
  SessionsListModel *m_listModel;
  FocusOverlay *m_focusOverlay;

  bool m_updatingActiveFields;

  void updateActiveTab();
  QString formatDuration(int ms) const;
};

#endif // MAINWINDOW_H
