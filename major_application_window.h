#ifndef MAJOR_APPLICATION_WINDOW_H
#define MAJOR_APPLICATION_WINDOW_H

#include <QMainWindow>

#include "database.h"
#include "user_session.h"

namespace Ui {
class MajorApplicationWindow;
}

class MajorApplicationWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MajorApplicationWindow(QWidget *parent = nullptr);
    ~MajorApplicationWindow();

private slots:
    void on_mainWindowPushButton_clicked();

    void on_cryptoWindowPushButton_clicked();

private:
    Ui::MajorApplicationWindow *ui;
};

#endif // MAJOR_APPLICATION_WINDOW_H
