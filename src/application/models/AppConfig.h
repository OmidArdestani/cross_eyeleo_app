#pragma once

namespace cross_eyeleop::application {

struct AppConfig {
    int focusSeconds{20 * 60};
    int shortBreakSeconds{20};
    int longBreakSeconds{5 * 60};
    int longBreakInterval{4};
};

} // namespace cross_eyeleop::application
