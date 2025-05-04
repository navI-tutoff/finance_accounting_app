#ifndef MAJOR_APPLICATION_WINDOW_H
#define MAJOR_APPLICATION_WINDOW_H

#include <QMainWindow>

#include "../data_structures/database.h"
#include "../data_structures/user_session.h"

#include "main_stacked_widget.h"
#include "crypto_stacked_widget.h"


namespace Ui {
class MajorApplicationWindow;
}

class MajorApplicationWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MajorApplicationWindow(QWidget *parent = nullptr);
    ~MajorApplicationWindow();

private slots:

private:
    Ui::MajorApplicationWindow *ui;

    MainStackedWidget *mainStackedWidget;
    CryptoStackedWidget *cryptoStackedWidget;
};

#endif // MAJOR_APPLICATION_WINDOW_H
