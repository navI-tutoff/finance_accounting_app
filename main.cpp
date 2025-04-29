#include "authorization_window.h"

#include <QApplication>
#include <QFile>

#include "database.h"


QString readTextFile(QString path) {
    QFile file(path);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        return in.readAll();
    }
    return "";
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString css = readTextFile(":/styles/styles/style.css");
    if (!css.isEmpty()) {
        a.setStyleSheet(css);
    }

    if (!Database::instance().connect("127.0.0.1", "testDB", "postgres", "danil890")) {
        qDebug() << "Fatal error while connecting database";
        return 1;
    }

    AuthorizationWindow w;
    w.show();
    return a.exec();
}
