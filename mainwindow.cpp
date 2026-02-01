#include "mainwindow.h"
#include "service.h"
#include "sessionslistmodel.h"
#include "sessionitemdelegate.h"
#include "createsessiondialog.h"
#include "focusoverlay.h"
#include "endsessiondialog.h"
#include <QFileDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>
#include <QMessageBox>
#include <QSettings>
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_service(new Service(this)),
      m_focusOverlay(nullptr),
      m_updatingActiveFields(false)
{
  ui = new Ui::MainWindow;
  ui->setupUi(this);
  setWindowTitle("flowy.adhd");

  m_listModel = new SessionsListModel(m_service, this);
  ui->listView->setModel(m_listModel);
  ui->listView->setItemDelegate(new SessionItemDelegate(this));

  // Connect Service signals
  connect(m_service, &Service::activeSessionChanged,
          this, &MainWindow::onActiveSessionChanged);
  connect(m_service, &Service::sessionTimerTicked,
          this, &MainWindow::onSessionTimerTicked);

  // Connect Active tab field changes
  connect(ui->oneLineEdit, &QLineEdit::editingFinished,
          this, &MainWindow::onActiveOnelinerChanged);

  // Debounce content text changes
  QTimer *contentDebounce = new QTimer(this);
  contentDebounce->setSingleShot(true);
  contentDebounce->setInterval(500);
  connect(ui->contentTextEdit, &QTextEdit::textChanged,
          [contentDebounce]() { contentDebounce->start(); });
  connect(contentDebounce, &QTimer::timeout,
          this, &MainWindow::onActiveContentChanged);

  // Set initial state: no active session
  ui->activeStackedWidget->setCurrentIndex(0);

  // Load saved vault directory from settings
  QSettings settings("flowy", "adhd");
  QString savedVaultPath = settings.value("vaultDirectory").toString();
  if (!savedVaultPath.isEmpty() && QDir(savedVaultPath).exists()) {
    m_service->setVaultDirectory(savedVaultPath);
    ui->vaultLabel->setText(savedVaultPath);
    // Switch to Sessions tab if we have a valid vault
    ui->tabWidget->setCurrentIndex(1);
  }
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
    ui->vaultLabel->setText(dir);

    // Save to settings for next launch
    QSettings settings("flowy", "adhd");
    settings.setValue("vaultDirectory", dir);
  }
}

void MainWindow::on_newSessionButton_clicked()
{
  CreateSessionDialog dialog(this);
  if (dialog.exec() == QDialog::Accepted) {
    QString oneliner = dialog.getOneliner();
    QString content = dialog.getContent();

    Session session = m_service->createSession(oneliner, content);
    m_service->startSession(session);
  }
}

void MainWindow::on_startSessionButton_clicked()
{
  QModelIndex index = ui->listView->currentIndex();
  if (!index.isValid()) {
    QMessageBox::warning(this, "No Selection", "Please select a session to start.");
    return;
  }

  Session session = m_listModel->getSessionByIndex(index);
  m_service->startSession(session);
}

void MainWindow::on_endSessionButton_clicked()
{
  auto activeSession = m_service->activeSession();
  if (!activeSession) {
    return;
  }

  EndSessionDialog dialog(activeSession->oneliner, m_service->getElapsedMs(), this);
  if (dialog.exec() == QDialog::Accepted) {
    Session::Result result = dialog.getResult();
    QString notes = dialog.getAdditionalNotes();

    // Append notes to content if provided
    if (!notes.isEmpty()) {
      Session updated = *activeSession;
      if (!updated.content.isEmpty()) {
        updated.content += "\n\n";
      }
      updated.content += notes;
      m_service->updateSession(updated);
    }

    m_service->endSession(result);
  }
}

void MainWindow::onActiveSessionChanged()
{
  auto activeSession = m_service->activeSession();

  if (activeSession) {
    // Session started
    ui->activeStackedWidget->setCurrentIndex(1);
    updateActiveTab();

    // Create and show overlay
    if (!m_focusOverlay) {
      m_focusOverlay = new FocusOverlay();
      connect(m_focusOverlay, &FocusOverlay::resetClicked,
              this, &MainWindow::onOverlayResetClicked);
      connect(m_focusOverlay, &FocusOverlay::endSessionClicked,
              this, &MainWindow::onOverlayEndSessionClicked);
    }

    m_focusOverlay->setOneliner(activeSession->oneliner);
    m_focusOverlay->setElapsedTime(0);
    m_focusOverlay->show();

    // Switch to Active tab
    ui->tabWidget->setCurrentIndex(2);
  } else {
    // Session ended
    ui->activeStackedWidget->setCurrentIndex(0);

    // Hide and delete overlay
    if (m_focusOverlay) {
      m_focusOverlay->hide();
      m_focusOverlay->deleteLater();
      m_focusOverlay = nullptr;
    }
  }
}

void MainWindow::onSessionTimerTicked(int elapsedMs)
{
  ui->elapsedLabel->setText(formatDuration(elapsedMs));

  if (m_focusOverlay) {
    m_focusOverlay->setElapsedTime(elapsedMs);
  }
}

void MainWindow::onActiveOnelinerChanged()
{
  if (m_updatingActiveFields) return;

  auto activeSession = m_service->activeSession();
  if (!activeSession) return;

  Session updated = *activeSession;
  updated.oneliner = ui->oneLineEdit->text();
  updated.lastModifiedAt = QDateTime::currentDateTime();

  m_updatingActiveFields = true;
  m_service->updateSession(updated);
  m_service->updateActiveSession(updated);

  // Update overlay
  if (m_focusOverlay) {
    m_focusOverlay->setOneliner(updated.oneliner);
  }

  m_updatingActiveFields = false;
}

void MainWindow::onActiveContentChanged()
{
  if (m_updatingActiveFields) return;

  auto activeSession = m_service->activeSession();
  if (!activeSession) return;

  Session updated = *activeSession;
  updated.content = ui->contentTextEdit->toPlainText();
  updated.lastModifiedAt = QDateTime::currentDateTime();

  m_updatingActiveFields = true;
  m_service->updateSession(updated);
  m_service->updateActiveSession(updated);
  m_updatingActiveFields = false;
}

void MainWindow::onOverlayResetClicked()
{
  QMessageBox::StandardButton reply = QMessageBox::question(
      this,
      "Reset Timer",
      "Are you sure you want to reset the timer to 00:00:00?",
      QMessageBox::Yes | QMessageBox::No);

  if (reply == QMessageBox::Yes) {
    m_service->resetSessionTimer();
  }
}

void MainWindow::onOverlayEndSessionClicked()
{
  on_endSessionButton_clicked();
}

void MainWindow::updateActiveTab()
{
  auto activeSession = m_service->activeSession();
  if (!activeSession) return;

  m_updatingActiveFields = true;

  ui->oneLineEdit->setText(activeSession->oneliner);
  ui->elapsedLabel->setText(formatDuration(m_service->getElapsedMs()));

  ui->createdLabel->setText(activeSession->createdAt.toString("yyyy-MM-dd hh:mm:ss"));
  ui->modifiedLabel->setText(activeSession->lastModifiedAt.toString("yyyy-MM-dd hh:mm:ss"));
  ui->contentTextEdit->setPlainText(activeSession->content);

  m_updatingActiveFields = false;
}

QString MainWindow::formatDuration(int ms) const
{
  int seconds = ms / 1000;
  int hours = seconds / 3600;
  int minutes = (seconds % 3600) / 60;
  int secs = seconds % 60;

  return QString("%1:%2:%3")
      .arg(hours, 2, 10, QChar('0'))
      .arg(minutes, 2, 10, QChar('0'))
      .arg(secs, 2, 10, QChar('0'));
}
