#ifndef FOCUSOVERLAY_H
#define FOCUSOVERLAY_H

#include <QWidget>

namespace Ui {
class FocusOverlay;
}

class FocusOverlay : public QWidget
{
  Q_OBJECT

public:
  explicit FocusOverlay(QWidget *parent = nullptr);
  ~FocusOverlay();

  void setOneliner(const QString &oneliner);
  void setElapsedTime(int elapsedMs);

signals:
  void resetClicked();
  void endSessionClicked();

private slots:
  void onResetButtonClicked();
  void onEndButtonClicked();

private:
  Ui::FocusOverlay *ui;
  void positionAtBottomCenter();
};

#endif // FOCUSOVERLAY_H
