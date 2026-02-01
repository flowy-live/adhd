#ifndef CREATESESSIONDIALOG_H
#define CREATESESSIONDIALOG_H

#include <QDialog>

namespace Ui {
class CreateSessionDialog;
}

class CreateSessionDialog : public QDialog
{
  Q_OBJECT

public:
  explicit CreateSessionDialog(QWidget *parent = nullptr);
  ~CreateSessionDialog();

  QString getOneliner() const;
  QString getContent() const;

private slots:
  void onOnelinerChanged(const QString &text);

private:
  Ui::CreateSessionDialog *ui;
};

#endif // CREATESESSIONDIALOG_H
