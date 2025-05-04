#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

class Database {
public:
    static Database& instance();

    bool connect(const QString &hostName, const QString &dbName,
                 const QString &userName, const QString &password, int port = 5432);

    void disconnect();

    bool isConnected();
    bool ping();
    bool reconnect();

    QSqlDatabase& database();
private:
    Database();
    ~Database();
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    QSqlDatabase m_database;
    QString m_hostName;
    QString m_dbName;
    QString m_userName;
    QString m_password;
    int m_port;
};

#endif // DATABASE_H
