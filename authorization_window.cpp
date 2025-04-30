#include "authorization_window.h"
#include "./ui_authorization_window.h"

AuthorizationWindow::AuthorizationWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::AuthorizationWindow) {
    ui->setupUi(this);

    ui->infoLabel->setStyleSheet("margin-left: auto;\nmargin-right: auto;");
}

AuthorizationWindow::~AuthorizationWindow() {
    delete ui;
}

void AuthorizationWindow::on_signInButton_clicked() {
    QString login = this->ui->loginLineEdit->text();
    QString password = this->ui->passwordLineEdit->text();

    // do hash for password
    QByteArray hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    password = hash.toHex();

    QSqlQuery singInQuery(Database::instance().database());
    singInQuery.prepare("SELECT id FROM users WHERE users.login = :login AND users.password = :password");
    singInQuery.bindValue(":login", login);
    singInQuery.bindValue(":password", password);
    if (singInQuery.exec()) {
        if (singInQuery.next()) {
            ui->infoLabel->setStyleSheet("color: green;");
            ui->infoLabel->setText("Вы успешно авторизовались");

            // open major app window and close auth window
            MajorApplicationWindow *majorApplicationWindow = new MajorApplicationWindow();
            majorApplicationWindow->show();

            UserSession::instance().setLogin(login);

            this->close();
        } else {
            ui->infoLabel->setStyleSheet("color: red;");
            ui->infoLabel->setText("Неверный логин или пароль");
        }
    }
}

void AuthorizationWindow::on_signUpButton_clicked() {
    QString login = this->ui->loginLineEdit->text();
    QString password = this->ui->passwordLineEdit->text();

    int loginMinLength = 6;
    if (login.length() < loginMinLength) {
        ui->infoLabel->setStyleSheet("color: #ea9c0b");
        ui->infoLabel->setText("Длина логина должна быть не менее " + QString::number(loginMinLength) + " символов");
        return;
    }

    bool hasUpper = QRegularExpression("[A-Z]").match(password).hasMatch();
    bool hasLower = QRegularExpression("[a-z]").match(password).hasMatch();
    bool hasDigit = QRegularExpression("[0-9]").match(password).hasMatch();
    //bool hasSpecial = QRegularExpression("[^a-zA-Z0-9]").match(password).hasMatch();
    int passwordMinLength = 8;
    bool isLongEnough = password.length() >= passwordMinLength;

    if (!hasUpper ||
        !hasLower ||
        !hasDigit ||
        //!hasSpecial ||
        !isLongEnough) {
        ui->infoLabel->setStyleSheet("color: #ea9c0b");
        ui->infoLabel->setText("Пароль должен содержать:\n"
                               "- Минимум " + QString::number(passwordMinLength) + " символов\n"
                               "- Заглавные и строчные буквы\n"
                               "- Цифры\n"
                               //"- Спецсимволы"
                               );
        return;
    }

    // do hash for password
    QByteArray hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    password = hash.toHex();

    QSqlQuery singUpQuery(Database::instance().database());
    singUpQuery.prepare("INSERT INTO users (login, password) VALUES (:login, :password);");
    singUpQuery.bindValue(":login", login);
    singUpQuery.bindValue(":password", password);
    if (singUpQuery.exec()) {
        ui->infoLabel->setStyleSheet("color: green");
        ui->infoLabel->setText("Вы успешно зарегистрировались");
    }
}
