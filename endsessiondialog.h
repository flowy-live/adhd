#ifndef ENDSESSIONDIALOG_H
#define ENDSESSIONDIALOG_H

#include <QDialog>
#include "service.h"

namespace Ui {
class EndSessionDialog;
}

class EndSessionDialog : public QDialog
{
  Q_OBJECT

public:
  explicit EndSessionDialog(const QString &oneliner, int elapsedMs, QWidget *parent = nullptr);
  ~EndSessionDialog();

  Session::Result getResult() const;
  QString getAdditionalNotes() const;

private slots:
  void onWinClicked();
  void onLossClicked();
  void onNeutralClicked();

private:
  Ui::EndSessionDialog *ui;
  Session::Result m_selectedResult;
};

#endif // ENDSESSIONDIALOG_H
