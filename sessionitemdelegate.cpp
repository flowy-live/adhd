#include "sessionitemdelegate.h"
#include "service.h"
#include <QPainter>
#include <QDateTime>

SessionItemDelegate::SessionItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void SessionItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                 const QModelIndex &index) const
{
  if (!index.isValid()) {
    QStyledItemDelegate::paint(painter, option, index);
    return;
  }

  painter->save();

  // Get session data
  Session session = index.data(Qt::UserRole).value<Session>();
  QString oneliner = session.oneliner;

  // Format duration
  int seconds = session.durationMs / 1000;
  int hours = seconds / 3600;
  int minutes = (seconds % 3600) / 60;
  int secs = seconds % 60;
  QString durationStr = QString("%1:%2:%3")
      .arg(hours, 2, 10, QChar('0'))
      .arg(minutes, 2, 10, QChar('0'))
      .arg(secs, 2, 10, QChar('0'));

  // Format result
  QString resultStr;
  switch (session.result) {
    case Session::WIN:
      resultStr = "WIN";
      break;
    case Session::LOSS:
      resultStr = "LOSS";
      break;
    case Session::NEUTRAL:
      resultStr = "NEUTRAL";
      break;
    default:
      resultStr = "UNSPECIFIED";
      break;
  }

  // Format relative time
  QDateTime now = QDateTime::currentDateTime();
  qint64 secsSinceModified = session.lastModifiedAt.secsTo(now);
  QString relativeTime;
  if (secsSinceModified < 60) {
    relativeTime = "just now";
  } else if (secsSinceModified < 3600) {
    relativeTime = QString("%1 min ago").arg(secsSinceModified / 60);
  } else if (secsSinceModified < 86400) {
    relativeTime = QString("%1 hr ago").arg(secsSinceModified / 3600);
  } else {
    relativeTime = QString("%1 days ago").arg(secsSinceModified / 86400);
  }

  QString metadata = QString("Duration: %1 | Result: %2 | Modified: %3")
      .arg(durationStr)
      .arg(resultStr)
      .arg(relativeTime);

  // Draw background
  if (option.state & QStyle::State_Selected) {
    painter->fillRect(option.rect, option.palette.highlight());
  } else {
    painter->fillRect(option.rect, option.palette.base());
  }

  // Draw result indicator bar on the left
  QColor indicatorColor;
  switch (session.result) {
    case Session::WIN:
      indicatorColor = QColor(46, 204, 113); // Green
      break;
    case Session::LOSS:
      indicatorColor = QColor(231, 76, 60); // Red
      break;
    case Session::NEUTRAL:
      indicatorColor = QColor(149, 165, 166); // Gray
      break;
    default:
      indicatorColor = QColor(0, 0, 0, 0); // Transparent
      break;
  }

  if (session.result != Session::UNSPECIFIED) {
    QRect indicatorRect(option.rect.left(), option.rect.top(), 4, option.rect.height());
    painter->fillRect(indicatorRect, indicatorColor);
  }

  // Set up text drawing
  QRect textRect = option.rect.adjusted(10, 5, -5, -5);
  QColor textColor = (option.state & QStyle::State_Selected)
      ? option.palette.highlightedText().color()
      : option.palette.text().color();

  // Draw oneliner (bold, larger font)
  painter->setPen(textColor);
  QFont onelineFont = option.font;
  onelineFont.setPointSize(14);
  onelineFont.setBold(true);
  painter->setFont(onelineFont);

  QRect onelineRect = textRect;
  onelineRect.setHeight(24);
  QString elidedOneliner = painter->fontMetrics().elidedText(oneliner, Qt::ElideRight, onelineRect.width());
  painter->drawText(onelineRect, Qt::AlignLeft | Qt::AlignVCenter, elidedOneliner);

  // Draw metadata (smaller font, lighter color)
  QFont metadataFont = option.font;
  metadataFont.setPointSize(11);
  painter->setFont(metadataFont);

  QColor metadataColor = textColor;
  metadataColor.setAlpha(180); // Make it slightly transparent
  painter->setPen(metadataColor);

  QRect metadataRect = textRect;
  metadataRect.setTop(onelineRect.bottom() + 2);
  QString elidedMetadata = painter->fontMetrics().elidedText(metadata, Qt::ElideRight, metadataRect.width());
  painter->drawText(metadataRect, Qt::AlignLeft | Qt::AlignVCenter, elidedMetadata);

  painter->restore();
}

QSize SessionItemDelegate::sizeHint(const QStyleOptionViewItem &option,
                                     const QModelIndex &index) const
{
  return QSize(option.rect.width(), 65);
}
