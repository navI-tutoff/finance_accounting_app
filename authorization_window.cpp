#include "authorization_window.h"
#include "./ui_authorization_window.h"

AuthorizationWindow::AuthorizationWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AuthorizationWindow)
{
    ui->setupUi(this);
}

AuthorizationWindow::~AuthorizationWindow()
{
    delete ui;
}
