#include <QApplication>
#include "app.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setApplicationName("CrossEyeLeoApp");
    QApplication::setApplicationVersion("1.0.0");
    QApplication::setOrganizationName("CrossEyeLeo");
    QApplication::setOrganizationDomain("crosseyeleo.example.com");
    QApplication::setQuitOnLastWindowClosed(false);

    App app;
    if (!app.init()) {
        return 0; // Another instance running
    }

    return a.exec();
}
