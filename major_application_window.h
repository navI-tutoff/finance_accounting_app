#ifndef MAJOR_APPLICATION_WINDOW_H
#define MAJOR_APPLICATION_WINDOW_H

#include <QMainWindow>

class MajorApplicationWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MajorApplicationWindow(QWidget *parent = nullptr);

signals:
};

#endif // MAJOR_APPLICATION_WINDOW_H
