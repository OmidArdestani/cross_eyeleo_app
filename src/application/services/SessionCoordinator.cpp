#include "SessionCoordinator.h"

#include <algorithm>

namespace cross_eyeleop::application {

SessionCoordinator::SessionCoordinator() = default;

void SessionCoordinator::configure(const AppConfig& config) {
    config_ = config;
    config_.focusSeconds = std::max(config_.focusSeconds, 1);
    config_.shortBreakSeconds = std::max(config_.shortBreakSeconds, 1);
    config_.longBreakSeconds = std::max(config_.longBreakSeconds, 1);
    config_.longBreakInterval = std::max(config_.longBreakInterval, 1);
    breakPolicy_ = domain::BreakSchedulePolicy({
        .shortBreakSeconds = config_.shortBreakSeconds,
        .longBreakSeconds = config_.longBreakSeconds,
        .longBreakInterval = config_.longBreakInterval,
    });
    stop();
}

void SessionCoordinator::start() {
    stateMachine_.start();
    snapshot_.running = true;
    snapshot_.completedFocusCycles = 0;
    beginFocus();
    publish();
}

void SessionCoordinator::stop() {
    stateMachine_.stop();
    snapshot_.running = false;
    snapshot_.onBreak = false;
    snapshot_.remainingSeconds = config_.focusSeconds;
    snapshot_.breakType = domain::BreakType::Short;
    publish();
}

void SessionCoordinator::tick() {
    if (!snapshot_.running) {
        return;
    }

    if (snapshot_.remainingSeconds > 0) {
        --snapshot_.remainingSeconds;
    }

    if (snapshot_.remainingSeconds == 0) {
        if (stateMachine_.mode() == domain::SessionMode::Focus) {
            ++snapshot_.completedFocusCycles;
            beginBreak();
        } else if (stateMachine_.mode() == domain::SessionMode::Break) {
            beginFocus();
        }
    }

    publish();
}

const SessionSnapshot& SessionCoordinator::snapshot() const {
    return snapshot_;
}

void SessionCoordinator::publish() {
    if (onSnapshotChanged) {
        onSnapshotChanged(snapshot_);
    }
}

void SessionCoordinator::beginFocus() {
    stateMachine_.toFocus();
    snapshot_.onBreak = false;
    snapshot_.remainingSeconds = config_.focusSeconds;
}

void SessionCoordinator::beginBreak() {
    stateMachine_.toBreak();
    snapshot_.onBreak = true;
    const auto breakSession = breakPolicy_.nextBreakForCycle(snapshot_.completedFocusCycles);
    snapshot_.breakType = breakSession.type;
    snapshot_.remainingSeconds = breakSession.durationSeconds;
}

} // namespace cross_eyeleop::application
