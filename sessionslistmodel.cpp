#include "sessionslistmodel.h"

SessionsListModel::SessionsListModel(Service *service, QObject *parent)
    : QAbstractListModel{parent}, m_service(service)
{
  connect(m_service, &Service::sessionsChanged, this, [this]() {
    beginResetModel();
    endResetModel();
  });
}

int SessionsListModel::rowCount(const QModelIndex &parent) const {
  return m_service->sessions().size();
}

int SessionsListModel::columnCount(const QModelIndex &parent) const {
  return 1;
}

QVariant SessionsListModel::data(const QModelIndex &index, int role) const {
  if (index.row() < 0 || index.row() >= rowCount()) {
    return QVariant();
  } else if (!index.isValid()) {
    return QVariant();
  }

  const Session session = m_service->sessions().at(index.row());
  switch (role) {
    case Qt::DisplayRole:
      return session.oneliner;
    default:
      return QVariant();
  }
}
