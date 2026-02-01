#ifndef SESSIONITEMDELEGATE_H
#define SESSIONITEMDELEGATE_H

#include <QStyledItemDelegate>

class SessionItemDelegate : public QStyledItemDelegate
{
  Q_OBJECT

public:
  explicit SessionItemDelegate(QObject *parent = nullptr);

  void paint(QPainter *painter, const QStyleOptionViewItem &option,
             const QModelIndex &index) const override;

  QSize sizeHint(const QStyleOptionViewItem &option,
                 const QModelIndex &index) const override;
};

#endif // SESSIONITEMDELEGATE_H
