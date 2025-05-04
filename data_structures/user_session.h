#ifndef USER_SESSION_H
#define USER_SESSION_H

#include <QString>

class UserSession {
public:
    static UserSession& instance();

    const QString& login() const;
    void setLogin(const QString &login);

    const int& id() const;
    void setId(const int &id);
private:
    UserSession() {};
    UserSession(const UserSession&) = delete;
    UserSession& operator=(const UserSession&) = delete;

    QString m_login;
    int m_id = -1;
};

#endif // USER_SESSION_H
