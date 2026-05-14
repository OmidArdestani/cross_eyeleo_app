#pragma once

#include <QObject>
#include <QLocalServer>
#include <QList>
#include <QTimer>

class SettingsManager;
class StateMachine;
class TaskScheduler;
class ActivityMonitor;
class TrayIcon;
class SettingsWindow;
class BigPauseWindow;
class MiniPauseWindow;
class NotificationWindow;
class BeforePauseWindow;

class App : public QObject
{
    Q_OBJECT

public:
    explicit App(QObject *parent = nullptr);
    ~App() override;

    bool init();

private slots:
    void onBigPauseDue();
    void onMiniPauseDue();
    void onWarningDue();
    void onUserInactive();
    void onUserReturned();

    void showSettings();
    void onPauseRequested(int minutes);
    void onResumeRequested();
    void onLongBreakRequested();

    void onBeforePauseReady();
    void onBeforePausePostpone();

    void onBigPauseSkipped();
    void onBigPauseCompleted();

    void onMiniPauseDismissed();
    void onMiniPauseCompleted();

    void onSettingsChanged();

private:
    void showBigPause();
    void showMiniPause();
    void pauseMonitoring(int minutes);
    void resumeMonitoring();

    SettingsManager  *m_settings{nullptr};
    StateMachine     *m_stateMachine{nullptr};
    TaskScheduler    *m_scheduler{nullptr};
    ActivityMonitor  *m_activityMonitor{nullptr};
    TrayIcon         *m_trayIcon{nullptr};

    SettingsWindow   *m_settingsWindow{nullptr};
    // One window per screen (or one window on primary in non-strict mode).
    QList<BigPauseWindow*>  m_bigPauseWindows;
    QList<MiniPauseWindow*> m_miniPauseWindows;
    NotificationWindow *m_notificationWindow{nullptr};
    BeforePauseWindow  *m_beforePauseWindow{nullptr};

    QLocalServer     *m_server{nullptr};
    QTimer           *m_resumeTimer{nullptr};

    bool              m_suspended{false};

    // Remaining time captured when user goes inactive so timers can be
    // resumed at the correct position when the user returns.
    qint64            m_inactiveBigPauseRemainingMs{-1};
    qint64            m_inactiveMiniPauseRemainingMs{-1};
};
