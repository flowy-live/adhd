#include "createsessiondialog.h"
#include "ui_createsessiondialog.h"
#include <QPushButton>

CreateSessionDialog::CreateSessionDialog(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::CreateSessionDialog)
{
  ui->setupUi(this);

  setWindowTitle("Create New Session");

  // Disable OK button initially
  ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

  // Connect oneliner field to validation
  connect(ui->onelinerEdit, &QLineEdit::textChanged,
          this, &CreateSessionDialog::onOnelinerChanged);
}

CreateSessionDialog::~CreateSessionDialog()
{
  delete ui;
}

QString CreateSessionDialog::getOneliner() const
{
  return ui->onelinerEdit->text().trimmed();
}

QString CreateSessionDialog::getContent() const
{
  return ui->contentEdit->toPlainText().trimmed();
}

void CreateSessionDialog::onOnelinerChanged(const QString &text)
{
  bool isValid = !text.trimmed().isEmpty();
  ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(isValid);
}
