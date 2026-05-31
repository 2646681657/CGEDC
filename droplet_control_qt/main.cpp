#include "mainwindow.h"
#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 加载深色工业风格表
    QFile qss(":/style/darkstyle.qss");
    if (qss.open(QFile::ReadOnly)) {
        app.setStyleSheet(qss.readAll());
        qss.close();
    }

    MainWindow w;
    w.show();
    return app.exec();
}
