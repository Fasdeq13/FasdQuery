#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QApplication::setApplicationName("FasdQuery");
    QApplication::setOrganizationName("FasdQuery");

    MainWindow window;
    window.show();

    return app.exec();
}
