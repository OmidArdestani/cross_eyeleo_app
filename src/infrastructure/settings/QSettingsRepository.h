#pragma once

#include <QSettings>

#include "application/interfaces/ISettingsRepository.h"

namespace cross_eyeleop::infrastructure {

class QSettingsRepository final : public application::ISettingsRepository {
public:
    application::AppConfig load() const override;
};

} // namespace cross_eyeleop::infrastructure
