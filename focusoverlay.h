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

protected:
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;

private:
  Ui::FocusOverlay *ui;
  void positionAtBottomCenter();

  // For dragging
  bool m_dragging;
  QPoint m_dragStartPosition;
};

#endif // FOCUSOVERLAY_H
