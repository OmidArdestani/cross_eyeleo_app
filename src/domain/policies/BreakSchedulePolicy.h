#pragma once

#include "domain/entities/BreakSession.h"

namespace cross_eyeleop::domain {

struct BreakPolicyConfig {
    int shortBreakSeconds{20};
    int longBreakSeconds{300};
    int longBreakInterval{4};
};

class BreakSchedulePolicy {
public:
    explicit BreakSchedulePolicy(BreakPolicyConfig config);

    BreakSession nextBreakForCycle(int completedFocusCycles) const;

private:
    BreakPolicyConfig config_{};
};

} // namespace cross_eyeleop::domain
