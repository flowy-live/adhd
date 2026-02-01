#ifndef SERVICE_H
#define SERVICE_H

#include <QList>
#include <QObject>
#include <QDateTime>
#include <QString>
#include <QFileInfo>
#include <QElapsedTimer>
#include <QTimer>
#include <optional>

struct Session
{
  enum Result {
    UNSPECIFIED = 0,
    WIN,
    LOSS,
    NEUTRAL
  };
  // Becomes the file name
  QString id;
  // Stored in the frontmatter under 'Oneliner' key
  QString oneliner;
  // Stored in the frontmatter
  int durationMs;
  // After the --- onwards, serves as general scratchpad
  QString content;
  // Stored in the frontmatter under 'Result' key
  Result result;

  QDateTime lastModifiedAt;
  QDateTime createdAt;

  bool isComplete() const;
};

class Service : public QObject {
  Q_OBJECT

public:
  explicit Service(QObject *parent = nullptr);
  ~Service();

  QList<Session> sessions() const;
  QString vaultDirectory() const;

  int sessionCount() const;
  int completedSessionCount() const;
  int totalWins() const;
  int totalLosses() const;
  int totalNeutrals() const;

public slots:
  /// @brief sets the directory from which to read/write sessions
  /// Fails silently if not a valid path
  /// Loads sessions from this path
  void setVaultDirectory(const QString &path);

  /// @brief creates the session and stores in sessions list
  /// @returns the newly created session
  Session createSession(const QString &oneliner, const QString &content);

  /// @brief updates the session
  /// Updates session in the sessions list, and the file associated
  void updateSession(const Session &session);

  void deleteSession(const QString &sessionId);

  // Session lifecycle methods
  void startSession(const Session &session);
  void updateActiveSession(const Session &session); // Update without restarting timer
  int getElapsedMs() const;
  void resetSessionTimer();
  void endSession(Session::Result result);
  std::optional<Session> activeSession() const;

signals:
  void sessionsChanged();
  void activeSessionChanged();
  void sessionTimerTicked(int elapsedMs);

private:
  QString m_vaultDirectory;
  QList<Session> m_sessions;

  // Active session tracking
  std::optional<Session> m_activeSession;
  QElapsedTimer m_sessionTimer;
  QTimer *m_tickTimer;

  /// @returns null Session if not a valid session markdown document
  std::optional<Session> parseFile(const QFileInfo &fileInfo);

  Session::Result parseResult(const QString &resultStr);
  void overwriteSessionFile(const Session &session);

private slots:
  void onTickTimerFired();
};

Q_DECLARE_METATYPE(Session)

#endif //SERVICE_H
