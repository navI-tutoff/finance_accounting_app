#include "database.h"

Database& Database::instance() {
    static Database instance;
    return instance;
}

Database::Database() {
    m_database = QSqlDatabase::addDatabase("QPSQL");
}

Database::~Database() {
    disconnect();
}

bool Database::connect(const QString &hostName, const QString &dbName,
                       const QString &userName, const QString &password, int port) {
    if (m_database.isOpen()) {
        qDebug() << "Database already connected";
        return true;
    }

    m_hostName = hostName;
    m_dbName = dbName;
    m_userName = userName;
    m_password = password;
    m_port = port;

    m_database.setHostName(m_hostName);
    m_database.setDatabaseName(m_dbName);
    m_database.setUserName(m_userName);
    m_database.setPassword(m_password);
    m_database.setPort(m_port);

    if (!m_database.open()) {
        qDebug() << "Database connect error. " << m_database.lastError().text();
        return false;
    }

    qDebug() << "Successfull database connection";
    return true;
}

void Database::disconnect() {
    if (m_database.isOpen()) {
        m_database.close();
        qDebug() << "Database connection closed";
    }
}

bool Database::isConnected() {
    return m_database.isOpen();
}

bool Database::ping() {
    if (!isConnected()) {
        qDebug() << "[Ping] There is no active connection";
        return false;
    }

    QSqlQuery query(m_database);
    if (query.exec("SELECT 1")) {
        return true;
    } else {
        qDebug() << "Ping do not successfull. " << query.lastError().text();
        return false;
    }
}

bool Database::reconnect()
{
    qDebug() << "Trying reconnect to database";
    disconnect();
    return connect(m_hostName, m_dbName, m_userName, m_password, m_port);
}

QSqlDatabase& Database::database()
{
    if (!ping()) {
        reconnect();
    }
    return m_database;
}
