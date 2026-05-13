#include "taskscheduler.h"

#include <QDateTime>

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
    m_bigPauseStartMs = QDateTime::currentMSecsSinceEpoch();
    m_bigPauseTimer->start(m_bigPauseIntervalMs);
}

void TaskScheduler::startMiniPauseTimer(int intervalMinutes)
{
    m_miniPauseIntervalMs = intervalMinutes * 60 * 1000;
    m_miniPauseStartMs = QDateTime::currentMSecsSinceEpoch();
    m_miniPauseTimer->start(m_miniPauseIntervalMs);
}

void TaskScheduler::startWarningTimer(int minutesBefore)
{
    if (!m_bigPauseTimer->isActive()) return;

    qint64 elapsed = QDateTime::currentMSecsSinceEpoch() - m_bigPauseStartMs;
    qint64 remaining = m_bigPauseIntervalMs - elapsed;
    qint64 warnMs = static_cast<qint64>(minutesBefore) * 60 * 1000;
    qint64 delay = remaining - warnMs;
    if (delay > 0) {
        m_warningTimer->start(static_cast<int>(delay));
    } else {
        // Fire immediately if we're already past warn point
        m_warningTimer->start(0);
    }
}

void TaskScheduler::resetBigPauseTimer()
{
    if (m_bigPauseIntervalMs > 0) {
        m_bigPauseStartMs = QDateTime::currentMSecsSinceEpoch();
        m_bigPauseTimer->start(m_bigPauseIntervalMs);
    }
}

void TaskScheduler::resetMiniPauseTimer()
{
    if (m_miniPauseIntervalMs > 0) {
        m_miniPauseStartMs = QDateTime::currentMSecsSinceEpoch();
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
    if (!m_bigPauseTimer->isActive()) return -1;
    qint64 elapsed = QDateTime::currentMSecsSinceEpoch() - m_bigPauseStartMs;
    return m_bigPauseIntervalMs - elapsed;
}

qint64 TaskScheduler::miniPauseRemainingMs() const
{
    if (!m_miniPauseTimer->isActive()) return -1;
    qint64 elapsed = QDateTime::currentMSecsSinceEpoch() - m_miniPauseStartMs;
    return m_miniPauseIntervalMs - elapsed;
}
