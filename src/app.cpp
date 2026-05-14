#include "app.h"

#include "settingsmanager.h"
#include "statemachine.h"
#include "taskscheduler.h"
#include "activitymonitor.h"
#include "trayicon.h"
#include "windows/settingswindow.h"
#include "windows/bigpausewindow.h"
#include "windows/minipausewindow.h"
#include "windows/notificationwindow.h"
#include "windows/beforepausewindow.h"
#include "exercises/exercisemanager.h"

#include <QApplication>
#include <QLocalSocket>
#include <QMessageBox>

App::App(QObject *parent)
    : QObject(parent)
{
    m_settings       = new SettingsManager(this);
    m_stateMachine   = new StateMachine(this);
    m_scheduler      = new TaskScheduler(this);
    m_activityMonitor = new ActivityMonitor(this);
    m_trayIcon       = new TrayIcon(this);
    m_resumeTimer    = new QTimer(this);
    m_resumeTimer->setSingleShot(true);
    connect(m_resumeTimer, &QTimer::timeout, this, &App::resumeMonitoring);
}

App::~App()
{
    if (m_server) {
        m_server->close();
    }
    m_settings->save();
}

bool App::init()
{
    // Single instance check
    m_server = new QLocalServer(this);
    if (!m_server->listen("CrossEyeLeoApp")) {
        QLocalSocket socket;
        socket.connectToServer("CrossEyeLeoApp");
        if (socket.waitForConnected(500)) {
            return false;
        }

        QLocalServer::removeServer("CrossEyeLeoApp");
        if (!m_server->listen("CrossEyeLeoApp")) {
            return false;
        }
    }

    m_settings->load();
    m_stateMachine->transitionTo(
        m_settings->firstLaunch() ? AppState::FIRST_LAUNCH : AppState::IDLE);

    if (m_settings->firstLaunch()) {
        m_settings->setFirstLaunch(false);
    }
    m_settings->save();

    // Connect tray signals
    connect(m_trayIcon, &TrayIcon::settingsRequested,  this, &App::showSettings);
    connect(m_trayIcon, &TrayIcon::aboutRequested,     this, &App::showAbout);
    connect(m_trayIcon, &TrayIcon::pauseRequested,     this, &App::onPauseRequested);
    connect(m_trayIcon, &TrayIcon::resumeRequested,    this, &App::onResumeRequested);
    connect(m_trayIcon, &TrayIcon::longBreakRequested, this, &App::onLongBreakRequested);
    connect(m_trayIcon, &TrayIcon::quitRequested,      qApp, &QApplication::quit);

    // Connect scheduler signals
    connect(m_scheduler, &TaskScheduler::bigPauseDue,  this, &App::onBigPauseDue);
    connect(m_scheduler, &TaskScheduler::miniPauseDue, this, &App::onMiniPauseDue);
    connect(m_scheduler, &TaskScheduler::warningDue,   this, &App::onWarningDue);

    // Connect activity monitor signals
    connect(m_activityMonitor, &ActivityMonitor::userInactive, this, &App::onUserInactive);
    connect(m_activityMonitor, &ActivityMonitor::userReturned, this, &App::onUserReturned);

    // Connect settings changed
    connect(m_settings, &SettingsManager::settingsChanged, this, &App::onSettingsChanged);

    // Start timers
    if (m_settings->bigPauseEnabled()) {
        m_scheduler->startBigPauseTimer(m_settings->bigPauseInterval());
        m_scheduler->startWarningTimer(m_settings->warningInterval());
    }
    if (m_settings->miniPauseEnabled())
        m_scheduler->startMiniPauseTimer(m_settings->miniPauseInterval());

    if (m_settings->inactivityTracking())
        m_activityMonitor->start();

    m_trayIcon->show();

    return true;
}

// ---------------------------------------------------------------------------
void App::onBigPauseDue()
{
    if (m_suspended) return;

    m_stateMachine->transitionTo(AppState::WAITING_SCREEN);

    if (m_settings->warningEnabled()) {
        if (!m_beforePauseWindow) {
            m_beforePauseWindow = new BeforePauseWindow();
            connect(m_beforePauseWindow, &BeforePauseWindow::readyClicked,
                    this, &App::onBeforePauseReady);
            connect(m_beforePauseWindow, &BeforePauseWindow::postponeClicked,
                    this, &App::onBeforePausePostpone);
        }
        m_beforePauseWindow->startCountdown(30);
        m_beforePauseWindow->show();
    } else {
        showBigPause();
    }
}

void App::onMiniPauseDue()
{
    if (m_suspended) return;
    if (m_stateMachine->isInBreak()) return;

    showMiniPause();
    m_scheduler->resetMiniPauseTimer();
}

void App::onWarningDue()
{
    if (m_suspended) return;

    qint64 remaining = m_scheduler->bigPauseRemainingMs();
    if (remaining < 0) remaining = 0;  // clamp: guard against stopped/late timers
    int minutes = static_cast<int>(remaining / 60000) + 1;

    if (!m_notificationWindow) {
        m_notificationWindow = new NotificationWindow(m_settings);
    }
    m_notificationWindow->showNotification(
        tr("Next long break in %1 minute(s)").arg(minutes));
}

void App::onUserInactive()
{
    // Capture remaining time so we can resume from where we left off.
    if (!m_suspended) {
        m_inactiveBigPauseRemainingMs  = m_scheduler->bigPauseRemainingMs();
        m_inactiveMiniPauseRemainingMs = m_scheduler->miniPauseRemainingMs();
        m_scheduler->stopAll();
    }
}

void App::onUserReturned()
{
    // Resume timers from the captured remaining time rather than restarting
    // the full configured interval, so a brief absence doesn't reset the schedule.
    if (!m_suspended) {
        if (m_settings->bigPauseEnabled()) {
            qint64 delay = (m_inactiveBigPauseRemainingMs > 0)
                           ? m_inactiveBigPauseRemainingMs
                           : static_cast<qint64>(m_settings->bigPauseInterval()) * 60 * 1000;
            m_scheduler->startBigPauseTimerMs(delay);
            m_scheduler->startWarningTimer(m_settings->warningInterval());
        }
        if (m_settings->miniPauseEnabled()) {
            qint64 delay = (m_inactiveMiniPauseRemainingMs > 0)
                           ? m_inactiveMiniPauseRemainingMs
                           : static_cast<qint64>(m_settings->miniPauseInterval()) * 60 * 1000;
            m_scheduler->startMiniPauseTimerMs(delay);
        }
        m_inactiveBigPauseRemainingMs  = -1;
        m_inactiveMiniPauseRemainingMs = -1;
    }
}

void App::showSettings()
{
    if (!m_settingsWindow) {
        m_settingsWindow = new SettingsWindow(m_settings);
        connect(m_settingsWindow, &SettingsWindow::tryShortBreak,
                this, &App::onMiniPauseDue);
        connect(m_settingsWindow, &SettingsWindow::tryLongBreak,
                this, &App::onBigPauseDue);
    }
    m_settingsWindow->loadFromSettings();
    m_settingsWindow->show();
    m_settingsWindow->raise();
    m_settingsWindow->activateWindow();
}

void App::showAbout()
{
    QMessageBox about;
    about.setWindowTitle(tr("About CrossEyeLeoApp"));
    about.setTextFormat(Qt::RichText);
    about.setText(
        "<b>CrossEyeLeoApp</b> v1.0.0<br/>"
        "A cross-platform eye exercise reminder.<br/><br/>"
        "<b>Developer:</b> Omid Ardestani<br/>"
        "Email: <a href=\"mailto:omid.91wo@gmail.com\">omid.91wo@gmail.com</a><br/>"
        "LinkedIn: <a href=\"https://www.linkedin.com/in/omidardestani/\">linkedin.com/in/omidardestani</a><br/>"
        "GitHub: <a href=\"https://github.com/OmidArdestani\">github.com/OmidArdestani</a>"
    );
    about.setStandardButtons(QMessageBox::Ok);
    about.exec();
}

void App::onPauseRequested(int minutes)
{
    pauseMonitoring(minutes);
}

void App::onResumeRequested()
{
    resumeMonitoring();
}

void App::onLongBreakRequested()
{
    onBigPauseDue();
}

void App::onBeforePauseReady()
{
    if (m_beforePauseWindow) m_beforePauseWindow->hide();
    showBigPause();
}

void App::onBeforePausePostpone()
{
    if (m_beforePauseWindow) m_beforePauseWindow->hide();
    m_settings->incrementPostponeCount();
    // Delay the next long break by 5 minutes using a one-off timer that does
    // NOT overwrite the configured m_bigPauseIntervalMs so subsequent breaks
    // still fire on the original schedule.
    m_scheduler->startBigPauseTimerMs(5LL * 60 * 1000);
    m_stateMachine->transitionTo(AppState::IDLE);
}

void App::onBigPauseSkipped()
{
    if (m_bigPauseWindow) m_bigPauseWindow->hideWithAnimation();
    m_settings->incrementSkipCount();
    m_settings->save();
    m_scheduler->resetBigPauseTimer();
    m_stateMachine->transitionTo(AppState::IDLE);
}

void App::onBigPauseCompleted()
{
    if (m_bigPauseWindow) m_bigPauseWindow->hideWithAnimation();
    m_settings->incrementLongBreakCount();
    m_settings->save();
    m_scheduler->resetBigPauseTimer();
    m_stateMachine->transitionTo(AppState::IDLE);
}

void App::onMiniPauseDismissed()
{
    if (m_miniPauseWindow) m_miniPauseWindow->hideWithAnimation();
}

void App::onMiniPauseCompleted()
{
    if (m_miniPauseWindow) m_miniPauseWindow->hideWithAnimation();
    m_settings->incrementShortBreakCount();
    m_settings->save();
}

void App::onSettingsChanged()
{
    m_scheduler->stopAll();
    if (m_settings->bigPauseEnabled())
        m_scheduler->startBigPauseTimer(m_settings->bigPauseInterval());
    if (m_settings->miniPauseEnabled())
        m_scheduler->startMiniPauseTimer(m_settings->miniPauseInterval());

    if (m_settings->inactivityTracking())
        m_activityMonitor->start();
    else
        m_activityMonitor->stop();
}

void App::showBigPause()
{
    if (!m_bigPauseWindow) {
        m_bigPauseWindow = new BigPauseWindow(m_settings);
        connect(m_bigPauseWindow, &BigPauseWindow::skipClicked,
                this, &App::onBigPauseSkipped);
        connect(m_bigPauseWindow, &BigPauseWindow::breakCompleted,
                this, &App::onBigPauseCompleted);
    }
    m_stateMachine->transitionTo(AppState::RELAXING);
    m_bigPauseWindow->startCountdown(m_settings->bigPauseDuration() * 60);
    m_bigPauseWindow->showWithAnimation();
}

void App::showMiniPause()
{
    if (!m_miniPauseWindow) {
        m_miniPauseWindow = new MiniPauseWindow(m_settings);
        connect(m_miniPauseWindow, &MiniPauseWindow::dismissed,
                this, &App::onMiniPauseDismissed);
        connect(m_miniPauseWindow, &MiniPauseWindow::completed,
                this, &App::onMiniPauseCompleted);
    }
    Exercise ex = ExerciseManager::randomExercise();
    m_miniPauseWindow->setExercise(ex.type, m_settings->miniPauseDuration());
    m_miniPauseWindow->showWithAnimation();
}

void App::pauseMonitoring(int minutes)
{
    // Always stop any pending resume timer before starting a new pause
    // so an "indefinite" pause is not cut short by a previous timed pause.
    m_resumeTimer->stop();
    m_suspended = true;
    m_scheduler->stopAll();
    m_trayIcon->setPaused(true);
    m_stateMachine->transitionTo(AppState::SUSPENDED);

    if (minutes > 0) {
        m_resumeTimer->start(minutes * 60 * 1000);
    }
}

void App::resumeMonitoring()
{
    m_resumeTimer->stop();  // cancel any pending timed resume
    m_suspended = false;
    m_trayIcon->setPaused(false);
    m_stateMachine->transitionTo(AppState::IDLE);

    if (m_settings->bigPauseEnabled())
        m_scheduler->startBigPauseTimer(m_settings->bigPauseInterval());
    if (m_settings->miniPauseEnabled())
        m_scheduler->startMiniPauseTimer(m_settings->miniPauseInterval());
}
