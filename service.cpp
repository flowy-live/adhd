#include "service.h"
#include <QDir>
#include <QFileInfoList>
#include <QUuid>
#include <QFile>

bool Session::isComplete() const {
  return result != UNSPECIFIED;
}

Service::Service(QObject *parent)
    : QObject(parent),
      m_tickTimer(new QTimer(this))
{
  m_tickTimer->setInterval(1000); // Fire every second
  connect(m_tickTimer, &QTimer::timeout, this, &Service::onTickTimerFired);
}

Service::~Service() {}

QString Service::vaultDirectory() const {
  return m_vaultDirectory;
}

int Service::sessionCount() const {
  return m_sessions.size();
}

int Service::completedSessionCount() const {
  int count = 0;
  for (const Session &session : m_sessions) {
    if (session.isComplete()) {
      count++;
    }
  }
  return count;
}

int Service::totalWins() const {
  int count = 0;
  for (const Session &session : m_sessions) {
    if (session.result == Session::WIN) {
      count++;
    }
  }
  return count;
}

int Service::totalLosses() const {
  int count = 0;
  for (const Session &session : m_sessions) {
    if (session.result == Session::LOSS) {
      count++;
    }
  }
  return count;
}

int Service::totalNeutrals() const {
  int count = 0;
  for (const Session &session : m_sessions) {
    if (session.result == Session::NEUTRAL) {
      count++;
    }
  }
  return count;
}

QList<Session> Service::sessions() const {
  return m_sessions;
}

void Service::setVaultDirectory(const QString &path) {
  m_sessions.clear();

  qDebug() << "Setting vault directory to: " << path;

  QDir dir = QDir(path);
  if (!dir.exists()) {
    qWarning() << "Vault directory does not exist";
    return;
  }

  m_vaultDirectory = path;

  dir.setFilter(QDir::Files);
  dir.setSorting(QDir::Time);

  QFileInfoList list = dir.entryInfoList();
  for (int i = 0; i < list.size(); ++i) {
    QFileInfo fileInfo = list.at(i);

    if (fileInfo.completeSuffix() != QStringLiteral("md")) {
      qWarning() << "Skipping invalid file format: " << fileInfo.completeSuffix();
      continue;
    }

    qDebug() << "Parsing file: " << fileInfo.absoluteFilePath();
    std::optional<Session> session = parseFile(fileInfo);
    if (session) {
      m_sessions.append(*session);
    } else {
      qWarning() << "Failed to parse session file: " << fileInfo.absoluteFilePath();
    }
  }

  emit sessionsChanged();
}

Session Service::createSession(const QString &oneliner, const QString &content) {
  assert(!m_vaultDirectory.isEmpty() && "Vault directory must be set before creating sessions");

  QString uuid = QUuid().toString(QUuid::WithoutBraces);

  Session session = {};
  session.id = uuid;
  session.oneliner = oneliner;
  session.durationMs = 0;
  session.content = content;
  session.result = Session::UNSPECIFIED;
  session.createdAt = QDateTime::currentDateTime();
  session.lastModifiedAt = QDateTime::currentDateTime();

  overwriteSessionFile(session);

  m_sessions.prepend(session);
  emit sessionsChanged();

  return session;
}

void Service::updateSession(const Session &session) {
  overwriteSessionFile(session);
  for (int i = 0; i < m_sessions.size(); ++i) {
    if (m_sessions[i].id == session.id) {
      m_sessions[i] = session;
      break;
    }
  }
  emit sessionsChanged();
}

void Service::deleteSession(const QString &sessionId) {
  QDir dir = QDir(m_vaultDirectory);
  QString filePath = dir.filePath(sessionId + ".md");
  QFile file = QFile(filePath);
  if (!file.remove()) {
    qWarning() << "Failed to delete session file: " << filePath;
    return;
  }

  for (int i = 0; i < m_sessions.size(); ++i) {
    if (m_sessions[i].id == sessionId) {
      m_sessions.removeAt(i);
      break;
    }
  }
  emit sessionsChanged();
}

std::optional<Session> Service::parseFile(const QFileInfo &fileInfo) {
  QString path = fileInfo.absoluteFilePath();
  QFile file = QFile(path);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return std::nullopt;
  }

  Session session = {};
  session.id = fileInfo.baseName(); // Get filename without extension
  session.content = "";
  session.oneliner = "";
  session.durationMs = 0;
  session.result = Session::UNSPECIFIED;
  session.createdAt = fileInfo.birthTime();
  session.lastModifiedAt = fileInfo.lastModified();

  QTextStream in(&file);
  int separatorCounts = 0;
  while (!in.atEnd()) {
    QString line = in.readLine();
    // qDebug() << "Reading line: " << line;

    if (separatorCounts >= 2) {
      session.content += line + '\n';
      continue;
    }

    switch (line[0].toLatin1()) {
      case '-':
        separatorCounts++;
        continue;
        break;
      default:
        break;
    }
    // parse each key value pair
    QStringList parts = line.split(':');
    if (parts.size() != 2) {
      continue;
    }
    QString key = parts[0].trimmed();
    QString value = parts[1].trimmed();
    if (key == "Oneliner") {
      session.oneliner = value;
      continue;
    } else if (key == "DurationMs") {
      session.durationMs = value.toInt();
      continue;
    } else if (key == "Result") {
      session.result = parseResult(value);
      continue;
    }
  }

  qDebug() << "Parsed oneliner: " << session.oneliner;
  qDebug() << "Parsed durationMs: " << session.durationMs;
  qDebug() << "Parsed result: " << static_cast<int>(session.result);
  qDebug() << "Parsed content: " << session.content;
  qDebug() << "Created at: " << session.createdAt;
  qDebug() << "Last modified at: " << session.lastModifiedAt;

  file.close();

  return session;
}

Session::Result Service::parseResult(const QString &resultStr) {
  if (resultStr == "Win") {
    return Session::WIN;
  } else if (resultStr == "Loss") {
    return Session::LOSS;
  } else if (resultStr == "Neutral") {
    return Session::NEUTRAL;
  } else {
    return Session::UNSPECIFIED;
  }
}

void Service::overwriteSessionFile(const Session &session)
{
    // rewrites the entire file
    QDir dir = QDir(m_vaultDirectory);
    QString filePath = dir.filePath(session.id + ".md");
    QFile file = QFile(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to update session file: " << filePath;
        return;
    }

    QTextStream out(&file);
    out << "---\n";
    out << "Oneliner: " << session.oneliner << "\n";
    out << "DurationMs: " << session.durationMs << "\n";
    QString resultStr;
    switch (session.result) {
    case Session::WIN:
      resultStr = "Win";
      break;
    case Session::LOSS:
      resultStr = "Loss";
      break;
    case Session::NEUTRAL:
      resultStr = "Neutral";
      break;
    default:
      resultStr = "";
      break;
    }
  if (!resultStr.isEmpty()) {
    out << "Result: " << resultStr << "\n";
  }
  out << "---\n";
  out << session.content << "\n";
  file.close();
}

// Session lifecycle methods

void Service::startSession(const Session &session) {
  if (m_activeSession.has_value()) {
    qWarning() << "A session is already active. End it before starting a new one.";
    return;
  }

  m_activeSession = session;
  m_sessionTimer.start();
  m_tickTimer->start();

  emit activeSessionChanged();
  emit sessionTimerTicked(0);
}

void Service::updateActiveSession(const Session &session) {
  if (!m_activeSession.has_value()) {
    qWarning() << "No active session to update.";
    return;
  }

  m_activeSession = session;
  // Don't emit activeSessionChanged - just update the session data
  // Timer keeps running
}

int Service::getElapsedMs() const {
  if (!m_activeSession) {
    return 0;
  }
  return m_sessionTimer.elapsed();
}

void Service::resetSessionTimer() {
  if (!m_activeSession) {
    return;
  }

  m_sessionTimer.restart();
  emit sessionTimerTicked(0);
}

void Service::endSession(Session::Result result) {
  if (!m_activeSession) {
    return;
  }

  // Update session with final values
  Session updated = *m_activeSession;
  updated.durationMs = getElapsedMs();
  updated.result = result;
  updated.lastModifiedAt = QDateTime::currentDateTime();

  // Save to file and update in list
  updateSession(updated);

  // Clear active state
  m_activeSession.reset();
  m_tickTimer->stop();

  emit activeSessionChanged();
}

std::optional<Session> Service::activeSession() const {
  return m_activeSession;
}

void Service::onTickTimerFired() {
  if (m_activeSession) {
    emit sessionTimerTicked(getElapsedMs());
  }
}
