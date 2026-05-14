#pragma once

#include <QString>
#include <QCoreApplication>

#ifdef Q_OS_WIN
#include <QSettings>
#endif

#ifndef APP_VERSION
#define APP_VERSION "unknown"
#endif

// Returns the installed application version.
// On Windows, reads from the registry key written by the installer
// (HKEY_CURRENT_USER\Software\CrossEyeLeoApp, value "Version").
// Falls back to QCoreApplication::applicationVersion() when the key
// is absent or on non-Windows platforms.
namespace AppVersion {
    inline QString get()
    {
#ifdef Q_OS_WIN
        QSettings reg("HKEY_CURRENT_USER\\Software\\CrossEyeLeoApp",
                       QSettings::NativeFormat);
        const QString regVersion = reg.value("Version").toString();
        if (!regVersion.isEmpty())
            return regVersion;
#endif
        return QCoreApplication::applicationVersion();
    }
} // namespace AppVersion
