#pragma once

#include <QString>
#include <QCoreApplication>
#include <QSettings>

#ifndef APP_VERSION
#define APP_VERSION "unknown"
#endif

// Returns the installed application version.
// Reads from the platform-appropriate location written by the installer:
//   - Windows: HKEY_CURRENT_USER\Software\CrossEyeLeoApp (registry)
//   - macOS:   ~/Library/Preferences/CrossEyeLeoApp.plist
//   - Linux:   ~/.config/CrossEyeLeoApp.conf
// Falls back to QCoreApplication::applicationVersion() when the
// installer-written entry is absent (e.g. dev builds).
namespace AppVersion {
    inline QString get()
    {
        QSettings settings(QSettings::NativeFormat, QSettings::UserScope,
                           "CrossEyeLeoApp");
        const QString v = settings.value("Version").toString();
        return v.isEmpty() ? QCoreApplication::applicationVersion() : v;
    }
} // namespace AppVersion
