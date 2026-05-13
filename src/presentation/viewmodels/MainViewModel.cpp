#include "MainViewModel.h"

#include <QTime>

namespace cross_eyeleop::presentation {

MainViewModel::MainViewModel(application::SessionCoordinator& coordinator,
                             infrastructure::QSettingsRepository& settingsRepository,
                             infrastructure::QtHeartbeat& heartbeat,
                             QObject* parent)
    : QObject(parent)
    , coordinator_(coordinator)
    , settingsRepository_(settingsRepository)
    , heartbeat_(heartbeat) {
    coordinator_.onSnapshotChanged = [this](const application::SessionSnapshot& snapshot) {
        onSnapshotChanged(snapshot);
    };

    coordinator_.configure(settingsRepository_.load());
    const auto snapshot = coordinator_.snapshot();
    onSnapshotChanged(snapshot);

    connect(&heartbeat_, &infrastructure::QtHeartbeat::pulse, this, [this] {
        coordinator_.tick();
    });
}

QString MainViewModel::statusText() const {
    return statusText_;
}

QString MainViewModel::countdownText() const {
    return countdownText_;
}

QString MainViewModel::buttonText() const {
    return buttonText_;
}

void MainViewModel::toggleSession() {
    if (coordinator_.snapshot().running) {
        heartbeat_.stop();
        coordinator_.stop();
        return;
    }

    coordinator_.start();
    heartbeat_.start();
}

void MainViewModel::onSnapshotChanged(const application::SessionSnapshot& snapshot) {
    statusText_ = snapshot.running ? (snapshot.onBreak ? "Break in progress" : "Focus in progress") : "Ready";
    countdownText_ = asTimerText(snapshot.remainingSeconds);
    buttonText_ = snapshot.running ? "Stop" : "Start";

    emit statusChanged(statusText_);
    emit countdownChanged(countdownText_);
    emit buttonTextChanged(buttonText_);
}

QString MainViewModel::asTimerText(int seconds) {
    return QTime(0, 0).addSecs(seconds).toString("mm:ss");
}

} // namespace cross_eyeleop::presentation
