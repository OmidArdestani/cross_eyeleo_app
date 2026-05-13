#pragma once

#include <QObject>
#include <QString>

#include "application/services/SessionCoordinator.h"
#include "infrastructure/settings/QSettingsRepository.h"
#include "infrastructure/timing/QtHeartbeat.h"

namespace cross_eyeleop::presentation {

class MainViewModel final : public QObject {
    Q_OBJECT

public:
    MainViewModel(application::SessionCoordinator& coordinator,
                  infrastructure::QSettingsRepository& settingsRepository,
                  infrastructure::QtHeartbeat& heartbeat,
                  QObject* parent = nullptr);

    [[nodiscard]] QString statusText() const;
    [[nodiscard]] QString countdownText() const;
    [[nodiscard]] QString buttonText() const;

public slots:
    void toggleSession();

signals:
    void statusChanged(const QString& value);
    void countdownChanged(const QString& value);
    void buttonTextChanged(const QString& value);

private:
    void onSnapshotChanged(const application::SessionSnapshot& snapshot);
    static QString asTimerText(int seconds);

    application::SessionCoordinator& coordinator_;
    infrastructure::QSettingsRepository& settingsRepository_;
    infrastructure::QtHeartbeat& heartbeat_;
    QString statusText_{"Ready"};
    QString countdownText_{"20:00"};
    QString buttonText_{"Start"};
};

} // namespace cross_eyeleop::presentation
