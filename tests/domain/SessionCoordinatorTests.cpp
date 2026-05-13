#include <iostream>

#include "application/services/SessionCoordinator.h"

int main() {
    using namespace cross_eyeleop::application;

    SessionCoordinator coordinator;
    coordinator.configure({
        .focusSeconds = 2,
        .shortBreakSeconds = 1,
        .longBreakSeconds = 3,
        .longBreakInterval = 2,
    });

    coordinator.start();
    if (!coordinator.snapshot().running || coordinator.snapshot().remainingSeconds != 2) {
        std::cerr << "start did not initialize focus session\n";
        return 1;
    }

    coordinator.tick();
    coordinator.tick();
    if (!coordinator.snapshot().onBreak || coordinator.snapshot().remainingSeconds != 1 ||
        coordinator.snapshot().breakType != cross_eyeleop::domain::BreakType::Short) {
        std::cerr << "short break transition failed\n";
        return 1;
    }

    coordinator.tick();
    if (coordinator.snapshot().onBreak || coordinator.snapshot().remainingSeconds != 2) {
        std::cerr << "break to focus transition failed\n";
        return 1;
    }

    coordinator.tick();
    coordinator.tick();
    if (!coordinator.snapshot().onBreak || coordinator.snapshot().remainingSeconds != 3 ||
        coordinator.snapshot().breakType != cross_eyeleop::domain::BreakType::Long) {
        std::cerr << "long break transition failed\n";
        return 1;
    }

    coordinator.stop();
    if (coordinator.snapshot().running) {
        std::cerr << "stop did not stop session\n";
        return 1;
    }

    return 0;
}
