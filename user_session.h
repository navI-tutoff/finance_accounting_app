#ifndef USER_SESSION_H
#define USER_SESSION_H

#include <QString>

class UserSession {
public:
    static UserSession& instance();

    void setLogin(const QString &login);
    const QString& login() const;
private:
    UserSession() {};
    UserSession(const UserSession&) = delete;
    UserSession& operator=(const UserSession&) = delete;

    QString m_login;
};

#endif // USER_SESSION_H
