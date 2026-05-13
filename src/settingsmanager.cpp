#include "settingsmanager.h"

SettingsManager::SettingsManager(QObject *parent)
    : QObject(parent)
    , m_qsettings(QSettings::UserScope, "CrossEyeLeop", "CrossEyeLeopApp")
{
}

void SettingsManager::load()
{
    m_bigPauseEnabled       = m_qsettings.value("bigPause/enabled",       true).toBool();
    m_bigPauseInterval      = m_qsettings.value("bigPause/interval",       60).toInt();
    m_bigPauseDuration      = m_qsettings.value("bigPause/duration",        5).toInt();

    m_miniPauseEnabled      = m_qsettings.value("miniPause/enabled",       true).toBool();
    m_miniPauseInterval     = m_qsettings.value("miniPause/interval",       10).toInt();
    m_miniPauseDuration     = m_qsettings.value("miniPause/duration",       20).toInt();
    m_miniPauseFullscreen   = m_qsettings.value("miniPause/fullscreen",  false).toBool();

    m_warningEnabled        = m_qsettings.value("warning/enabled",          true).toBool();
    m_warningInterval       = m_qsettings.value("warning/interval",            1).toInt();

    m_soundsEnabled         = m_qsettings.value("other/sounds",            true).toBool();
    m_strictMode            = m_qsettings.value("other/strictMode",       false).toBool();
    m_windowNearby          = m_qsettings.value("other/windowNearby",      true).toBool();
    m_inactivityTracking    = m_qsettings.value("other/inactivityTracking",true).toBool();
    m_canCloseNotifications = m_qsettings.value("other/canCloseNotif",     true).toBool();
    m_showNotifications     = m_qsettings.value("other/showNotif",         true).toBool();
    m_firstLaunch           = m_qsettings.value("other/firstLaunch",       true).toBool();

    m_shortBreakCount  = m_qsettings.value("stats/shortBreak",   0).toInt();
    m_longBreakCount   = m_qsettings.value("stats/longBreak",    0).toInt();
    m_skipCount        = m_qsettings.value("stats/skip",         0).toInt();
    m_postponeCount    = m_qsettings.value("stats/postpone",     0).toInt();
    m_refuseCount      = m_qsettings.value("stats/refuse",       0).toInt();
}

void SettingsManager::save()
{
    m_qsettings.setValue("bigPause/enabled",         m_bigPauseEnabled);
    m_qsettings.setValue("bigPause/interval",        m_bigPauseInterval);
    m_qsettings.setValue("bigPause/duration",        m_bigPauseDuration);

    m_qsettings.setValue("miniPause/enabled",        m_miniPauseEnabled);
    m_qsettings.setValue("miniPause/interval",       m_miniPauseInterval);
    m_qsettings.setValue("miniPause/duration",       m_miniPauseDuration);
    m_qsettings.setValue("miniPause/fullscreen",     m_miniPauseFullscreen);

    m_qsettings.setValue("warning/enabled",          m_warningEnabled);
    m_qsettings.setValue("warning/interval",         m_warningInterval);

    m_qsettings.setValue("other/sounds",             m_soundsEnabled);
    m_qsettings.setValue("other/strictMode",         m_strictMode);
    m_qsettings.setValue("other/windowNearby",       m_windowNearby);
    m_qsettings.setValue("other/inactivityTracking", m_inactivityTracking);
    m_qsettings.setValue("other/canCloseNotif",      m_canCloseNotifications);
    m_qsettings.setValue("other/showNotif",          m_showNotifications);
    m_qsettings.setValue("other/firstLaunch",        m_firstLaunch);

    m_qsettings.setValue("stats/shortBreak",  m_shortBreakCount);
    m_qsettings.setValue("stats/longBreak",   m_longBreakCount);
    m_qsettings.setValue("stats/skip",        m_skipCount);
    m_qsettings.setValue("stats/postpone",    m_postponeCount);
    m_qsettings.setValue("stats/refuse",      m_refuseCount);

    m_qsettings.sync();
}
