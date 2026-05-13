#include "QSettingsRepository.h"

namespace cross_eyeleop::infrastructure {

application::AppConfig QSettingsRepository::load() const {
    QSettings settings("cross_eyeleop_app", "cross_eyeleop_app");
    application::AppConfig config;
    config.focusSeconds = settings.value("focusSeconds", config.focusSeconds).toInt();
    config.shortBreakSeconds = settings.value("shortBreakSeconds", config.shortBreakSeconds).toInt();
    config.longBreakSeconds = settings.value("longBreakSeconds", config.longBreakSeconds).toInt();
    config.longBreakInterval = settings.value("longBreakInterval", config.longBreakInterval).toInt();
    return config;
}

} // namespace cross_eyeleop::infrastructure
