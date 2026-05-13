#include "taskscheduler.h"

TaskScheduler::TaskScheduler(QObject *parent)
    : QObject(parent)
{
    m_bigPauseTimer  = new QTimer(this);
    m_miniPauseTimer = new QTimer(this);
    m_warningTimer   = new QTimer(this);

    m_bigPauseTimer->setSingleShot(true);
    m_miniPauseTimer->setSingleShot(true);
    m_warningTimer->setSingleShot(true);

    connect(m_bigPauseTimer,  &QTimer::timeout, this, &TaskScheduler::bigPauseDue);
    connect(m_miniPauseTimer, &QTimer::timeout, this, &TaskScheduler::miniPauseDue);
    connect(m_warningTimer,   &QTimer::timeout, this, &TaskScheduler::warningDue);
}

void TaskScheduler::startBigPauseTimer(int intervalMinutes)
{
    m_bigPauseIntervalMs = intervalMinutes * 60 * 1000;
    m_bigPauseTimer->start(m_bigPauseIntervalMs);
}

void TaskScheduler::startMiniPauseTimer(int intervalMinutes)
{
    m_miniPauseIntervalMs = intervalMinutes * 60 * 1000;
    m_miniPauseTimer->start(m_miniPauseIntervalMs);
}

void TaskScheduler::startBigPauseTimerMs(qint64 delayMs)
{
    // Stops warning timer; starts big-pause for the given delay
    // without modifying the configured m_bigPauseIntervalMs so that
    // the next resetBigPauseTimer() still uses the original interval.
    m_warningTimer->stop();
    m_bigPauseTimer->start(static_cast<int>(delayMs));
}

void TaskScheduler::startMiniPauseTimerMs(qint64 delayMs)
{
    m_miniPauseTimer->start(static_cast<int>(delayMs));
}

void TaskScheduler::startWarningTimer(int minutesBefore)
{
    int remaining = m_bigPauseTimer->remainingTime();
    if (remaining < 0) return;  // big-pause timer not active

    qint64 warnMs = static_cast<qint64>(minutesBefore) * 60 * 1000;
    qint64 delay  = static_cast<qint64>(remaining) - warnMs;
    if (delay > 0) {
        m_warningTimer->start(static_cast<int>(delay));
    } else {
        // Already past the warn point — fire immediately
        m_warningTimer->start(0);
    }
}

void TaskScheduler::resetBigPauseTimer()
{
    if (m_bigPauseIntervalMs > 0) {
        m_bigPauseTimer->start(m_bigPauseIntervalMs);
    }
}

void TaskScheduler::resetMiniPauseTimer()
{
    if (m_miniPauseIntervalMs > 0) {
        m_miniPauseTimer->start(m_miniPauseIntervalMs);
    }
}

void TaskScheduler::stopAll()
{
    m_bigPauseTimer->stop();
    m_miniPauseTimer->stop();
    m_warningTimer->stop();
}

qint64 TaskScheduler::bigPauseRemainingMs() const
{
    int r = m_bigPauseTimer->remainingTime();
    return (r < 0) ? -1LL : static_cast<qint64>(r);
}

qint64 TaskScheduler::miniPauseRemainingMs() const
{
    int r = m_miniPauseTimer->remainingTime();
    return (r < 0) ? -1LL : static_cast<qint64>(r);
}
