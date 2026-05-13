#include "BreakSchedulePolicy.h"

#include <algorithm>

namespace cross_eyeleop::domain {

BreakSchedulePolicy::BreakSchedulePolicy(BreakPolicyConfig config)
    : config_(config) {
    config_.shortBreakSeconds = std::max(config_.shortBreakSeconds, 1);
    config_.longBreakSeconds = std::max(config_.longBreakSeconds, 1);
    config_.longBreakInterval = std::max(config_.longBreakInterval, 1);
}

BreakSession BreakSchedulePolicy::nextBreakForCycle(int completedFocusCycles) const {
    const bool isLongBreak = completedFocusCycles > 0 && (completedFocusCycles % config_.longBreakInterval == 0);
    if (isLongBreak) {
        return BreakSession{BreakType::Long, config_.longBreakSeconds};
    }
    return BreakSession{BreakType::Short, config_.shortBreakSeconds};
}

} // namespace cross_eyeleop::domain
