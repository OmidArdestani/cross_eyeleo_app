#pragma once

#include "BreakType.h"

namespace cross_eyeleop::domain {

struct BreakSession {
    BreakType type{BreakType::Short};
    int durationSeconds{0};
};

} // namespace cross_eyeleop::domain
