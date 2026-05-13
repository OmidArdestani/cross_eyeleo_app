#include <QApplication>
#include <QFont>
#include "app.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Avoid "missing font family 'Sans-serif'" warning by setting an explicit
    // font with known fallback families rather than relying on the platform
    // default which may resolve to an unavailable family name.
    QFont appFont;
    appFont.setFamilies({"DejaVu Sans", "Arial", "Helvetica Neue", "Helvetica"});
    QApplication::setFont(appFont);

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
