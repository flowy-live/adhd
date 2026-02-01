#include "focusoverlay.h"
#include "ui_focusoverlay.h"
#include <QScreen>
#include <QGuiApplication>

FocusOverlay::FocusOverlay(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::FocusOverlay)
{
  ui->setupUi(this);

  // Set window flags for always-on-top frameless window
  // Use Qt::Window instead of Qt::Tool so it stays visible when app loses focus
  setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
  setAttribute(Qt::WA_TranslucentBackground);
  setAttribute(Qt::WA_ShowWithoutActivating); // Don't steal focus when showing

  // Connect buttons
  connect(ui->resetButton, &QPushButton::clicked,
          this, &FocusOverlay::onResetButtonClicked);
  connect(ui->endButton, &QPushButton::clicked,
          this, &FocusOverlay::onEndButtonClicked);

  // Position window
  positionAtBottomCenter();
}

FocusOverlay::~FocusOverlay()
{
  delete ui;
}

void FocusOverlay::setOneliner(const QString &oneliner)
{
  ui->onelinerLabel->setText(oneliner);
}

void FocusOverlay::setElapsedTime(int elapsedMs)
{
  int seconds = elapsedMs / 1000;
  int hours = seconds / 3600;
  int minutes = (seconds % 3600) / 60;
  int secs = seconds % 60;

  QString timeStr = QString("%1:%2:%3")
      .arg(hours, 2, 10, QChar('0'))
      .arg(minutes, 2, 10, QChar('0'))
      .arg(secs, 2, 10, QChar('0'));

  ui->elapsedLabel->setText("Elapsed: " + timeStr);
}

void FocusOverlay::onResetButtonClicked()
{
  emit resetClicked();
}

void FocusOverlay::onEndButtonClicked()
{
  emit endSessionClicked();
}

void FocusOverlay::positionAtBottomCenter()
{
  QScreen *screen = QGuiApplication::primaryScreen();
  if (!screen) return;

  QRect screenGeometry = screen->geometry();
  int x = (screenGeometry.width() - width()) / 2;
  int y = screenGeometry.height() - height();

  move(x, y);
}
