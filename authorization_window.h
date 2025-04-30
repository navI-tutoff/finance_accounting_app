#ifndef AUTHORIZATION_WINDOW_H
#define AUTHORIZATION_WINDOW_H

#include <QMainWindow>
#include <QSqlQuery>

#include <QCryptographicHash>
#include <QRegularExpression>

#include "major_application_window.h"
#include "database.h"
#include "user_session.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class AuthorizationWindow;
}
QT_END_NAMESPACE

class AuthorizationWindow : public QMainWindow
{
    Q_OBJECT

public:
    AuthorizationWindow(QWidget *parent = nullptr);
    ~AuthorizationWindow();

private slots:

    void on_signInButton_clicked();

    void on_signUpButton_clicked();

private:
    Ui::AuthorizationWindow *ui;
};
#endif // AUTHORIZATION_WINDOW_H
