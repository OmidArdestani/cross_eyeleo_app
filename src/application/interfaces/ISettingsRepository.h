#pragma once

#include "application/models/AppConfig.h"

namespace cross_eyeleop::application {

class ISettingsRepository {
public:
    virtual ~ISettingsRepository() = default;
    virtual AppConfig load() const = 0;
};

} // namespace cross_eyeleop::application
