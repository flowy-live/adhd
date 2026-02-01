#ifndef SESSIONSLISTMODEL_H
#define SESSIONSLISTMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include "service.h"

class SessionsListModel : public QAbstractListModel
{
  Q_OBJECT
public:
  explicit SessionsListModel(Service *service, QObject *parent = nullptr);

  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  int columnCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index, int role) const override;

  Session getSessionByIndex(const QModelIndex &index) const;

private:
  Service *m_service;
};

#endif // SESSIONSLISTMODEL_H
