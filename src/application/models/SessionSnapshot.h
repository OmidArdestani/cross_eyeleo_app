#pragma once

#include "domain/entities/BreakType.h"

namespace cross_eyeleop::application {

struct SessionSnapshot {
    bool running{false};
    bool onBreak{false};
    int remainingSeconds{0};
    int completedFocusCycles{0};
    domain::BreakType breakType{domain::BreakType::Short};
};

} // namespace cross_eyeleop::application
