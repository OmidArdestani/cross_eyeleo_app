#pragma once

#include <functional>

#include "application/models/AppConfig.h"
#include "application/models/SessionSnapshot.h"
#include "domain/policies/BreakSchedulePolicy.h"
#include "domain/services/SessionStateMachine.h"

namespace cross_eyeleop::application {

class SessionCoordinator {
public:
    using SnapshotCallback = std::function<void(const SessionSnapshot&)>;

    SessionCoordinator();

    void configure(const AppConfig& config);
    void start();
    void stop();
    void tick();

    [[nodiscard]] const SessionSnapshot& snapshot() const;

    SnapshotCallback onSnapshotChanged;

private:
    void publish();
    void beginFocus();
    void beginBreak();

    AppConfig config_{};
    domain::BreakSchedulePolicy breakPolicy_{{}};
    domain::SessionStateMachine stateMachine_{};
    SessionSnapshot snapshot_{};
};

} // namespace cross_eyeleop::application
