#pragma once

#include <QObject>
#include <QSettings>

class SettingsManager : public QObject
{
    Q_OBJECT

public:
    explicit SettingsManager(QObject *parent = nullptr);

    void load();
    void save();

    // Big pause
    bool bigPauseEnabled() const { return m_bigPauseEnabled; }
    void setBigPauseEnabled(bool v) { m_bigPauseEnabled = v; emit settingsChanged(); }

    int bigPauseInterval() const { return m_bigPauseInterval; }
    void setBigPauseInterval(int v) { m_bigPauseInterval = v; emit settingsChanged(); }

    int bigPauseDuration() const { return m_bigPauseDuration; }
    void setBigPauseDuration(int v) { m_bigPauseDuration = v; emit settingsChanged(); }

    // Mini pause
    bool miniPauseEnabled() const { return m_miniPauseEnabled; }
    void setMiniPauseEnabled(bool v) { m_miniPauseEnabled = v; emit settingsChanged(); }

    int miniPauseInterval() const { return m_miniPauseInterval; }
    void setMiniPauseInterval(int v) { m_miniPauseInterval = v; emit settingsChanged(); }

    int miniPauseDuration() const { return m_miniPauseDuration; }
    void setMiniPauseDuration(int v) { m_miniPauseDuration = v; emit settingsChanged(); }

    bool miniPauseFullscreen() const { return m_miniPauseFullscreen; }
    void setMiniPauseFullscreen(bool v) { m_miniPauseFullscreen = v; emit settingsChanged(); }

    // Warning
    bool warningEnabled() const { return m_warningEnabled; }
    void setWarningEnabled(bool v) { m_warningEnabled = v; emit settingsChanged(); }

    int warningInterval() const { return m_warningInterval; }
    void setWarningInterval(int v) { m_warningInterval = v; emit settingsChanged(); }

    // Other
    bool soundsEnabled() const { return m_soundsEnabled; }
    void setSoundsEnabled(bool v) { m_soundsEnabled = v; emit settingsChanged(); }

    bool strictMode() const { return m_strictMode; }
    void setStrictMode(bool v) { m_strictMode = v; emit settingsChanged(); }

    bool windowNearby() const { return m_windowNearby; }
    void setWindowNearby(bool v) { m_windowNearby = v; emit settingsChanged(); }

    bool inactivityTracking() const { return m_inactivityTracking; }
    void setInactivityTracking(bool v) { m_inactivityTracking = v; emit settingsChanged(); }

    bool canCloseNotifications() const { return m_canCloseNotifications; }
    void setCanCloseNotifications(bool v) { m_canCloseNotifications = v; emit settingsChanged(); }

    bool showNotifications() const { return m_showNotifications; }
    void setShowNotifications(bool v) { m_showNotifications = v; emit settingsChanged(); }

    bool firstLaunch() const { return m_firstLaunch; }
    void setFirstLaunch(bool v) { m_firstLaunch = v; }

    // Statistics
    int shortBreakCount() const { return m_shortBreakCount; }
    void setShortBreakCount(int v) { m_shortBreakCount = v; }
    void incrementShortBreakCount() { ++m_shortBreakCount; }

    int longBreakCount() const { return m_longBreakCount; }
    void setLongBreakCount(int v) { m_longBreakCount = v; }
    void incrementLongBreakCount() { ++m_longBreakCount; }

    int skipCount() const { return m_skipCount; }
    void setSkipCount(int v) { m_skipCount = v; }
    void incrementSkipCount() { ++m_skipCount; }

    int postponeCount() const { return m_postponeCount; }
    void setPostponeCount(int v) { m_postponeCount = v; }
    void incrementPostponeCount() { ++m_postponeCount; }

    int refuseCount() const { return m_refuseCount; }
    void setRefuseCount(int v) { m_refuseCount = v; }
    void incrementRefuseCount() { ++m_refuseCount; }

signals:
    void settingsChanged();

private:
    QSettings m_qsettings;

    bool m_bigPauseEnabled{true};
    int  m_bigPauseInterval{60};
    int  m_bigPauseDuration{5};

    bool m_miniPauseEnabled{true};
    int  m_miniPauseInterval{10};
    int  m_miniPauseDuration{20};
    bool m_miniPauseFullscreen{false};

    bool m_warningEnabled{true};
    int  m_warningInterval{1};

    bool m_soundsEnabled{true};
    bool m_strictMode{false};
    bool m_windowNearby{true};
    bool m_inactivityTracking{true};
    bool m_canCloseNotifications{true};
    bool m_showNotifications{true};
    bool m_firstLaunch{true};

    int m_shortBreakCount{0};
    int m_longBreakCount{0};
    int m_skipCount{0};
    int m_postponeCount{0};
    int m_refuseCount{0};
};
