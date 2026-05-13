#include <QApplication>
#include <QTimer>

#include "application/services/SessionCoordinator.h"
#include "infrastructure/settings/QSettingsRepository.h"
#include "infrastructure/timing/QtHeartbeat.h"
#include "presentation/viewmodels/MainViewModel.h"
#include "presentation/windows/MainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    cross_eyeleop::application::SessionCoordinator coordinator;
    cross_eyeleop::infrastructure::QSettingsRepository settingsRepository;
    cross_eyeleop::infrastructure::QtHeartbeat heartbeat;
    cross_eyeleop::presentation::MainViewModel viewModel(coordinator, settingsRepository, heartbeat);
    cross_eyeleop::presentation::MainWindow window(viewModel);
    window.show();

    const auto screenshotPath = qEnvironmentVariable("CROSSEYELEOP_SCREENSHOT_PATH");
    if (!screenshotPath.isEmpty()) {
        QTimer::singleShot(250, &app, [&window, &app, screenshotPath] {
            window.grab().save(screenshotPath);
            app.quit();
        });
    }

    return app.exec();
}
