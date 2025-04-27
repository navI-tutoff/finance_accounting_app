#ifndef AUTHORIZATION_WINDOW_H
#define AUTHORIZATION_WINDOW_H

#include <QMainWindow>

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

private:
    Ui::AuthorizationWindow *ui;
};
#endif // AUTHORIZATION_WINDOW_H
