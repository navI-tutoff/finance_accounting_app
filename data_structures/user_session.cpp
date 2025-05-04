#include "user_session.h"

UserSession& UserSession::instance() {
    static UserSession instance;
    return instance;
}

void UserSession::setLogin(const QString& login) {
    this->m_login = login;
}

const QString& UserSession::login() const {
    return this->m_login;
}

void UserSession::setId(const int &id) {
    this->m_id = id;
}

const int& UserSession::id() const {
    return this->m_id;
}
