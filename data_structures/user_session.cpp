#include "user_session.h"

UserSession& UserSession::instance() {
    static UserSession instance;
    return instance;
}

void UserSession::setLogin(const QString& login) {
    this->m_login = login;
}

const QString& UserSession::login() const {
    return m_login;
}
