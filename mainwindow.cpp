#include "mainwindow.h"
#include "service.h"
#include "sessionslistmodel.h"
#include <QFileDialog>
#include <QLabel>
#include <QVBoxLayout>
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_service(new Service(this))
{
  ui = new Ui::MainWindow;
  ui->setupUi(this);

  m_listModel = new SessionsListModel(m_service, this);
  ui->listView->setModel(m_listModel);
}

MainWindow::~MainWindow() {}

void MainWindow::on_selectVaultButton_clicked()
{
  QString dir = QFileDialog::getExistingDirectory(this,
                                                  "Select Directory",
                                                  QDir::homePath(),
                                                  QFileDialog::ShowDirsOnly);
  if (!dir.isEmpty()) {
    m_service->setVaultDirectory(dir);
    ui->tabWidget->setCurrentIndex(1);
    ui->vaultLabel->setText(m_service->vaultDirectory());
  }
}
