#include "endsessiondialog.h"
#include "ui_endsessiondialog.h"

EndSessionDialog::EndSessionDialog(const QString &oneliner, int elapsedMs, QWidget *parent)
    : QDialog(parent),
      ui(new Ui::EndSessionDialog),
      m_selectedResult(Session::UNSPECIFIED)
{
  ui->setupUi(this);

  setWindowTitle("End Session");

  // Set oneliner and elapsed time
  ui->onelinerLabel->setText(oneliner);

  int seconds = elapsedMs / 1000;
  int hours = seconds / 3600;
  int minutes = (seconds % 3600) / 60;
  int secs = seconds % 60;

  QString timeStr = QString("%1:%2:%3")
      .arg(hours, 2, 10, QChar('0'))
      .arg(minutes, 2, 10, QChar('0'))
      .arg(secs, 2, 10, QChar('0'));

  ui->elapsedLabel->setText("Duration: " + timeStr);

  // Connect buttons
  connect(ui->winButton, &QPushButton::clicked,
          this, &EndSessionDialog::onWinClicked);
  connect(ui->lossButton, &QPushButton::clicked,
          this, &EndSessionDialog::onLossClicked);
  connect(ui->neutralButton, &QPushButton::clicked,
          this, &EndSessionDialog::onNeutralClicked);
}

EndSessionDialog::~EndSessionDialog()
{
  delete ui;
}

Session::Result EndSessionDialog::getResult() const
{
  return m_selectedResult;
}

QString EndSessionDialog::getAdditionalNotes() const
{
  return ui->notesEdit->toPlainText().trimmed();
}

void EndSessionDialog::onWinClicked()
{
  m_selectedResult = Session::WIN;
  accept();
}

void EndSessionDialog::onLossClicked()
{
  m_selectedResult = Session::LOSS;
  accept();
}

void EndSessionDialog::onNeutralClicked()
{
  m_selectedResult = Session::NEUTRAL;
  accept();
}
