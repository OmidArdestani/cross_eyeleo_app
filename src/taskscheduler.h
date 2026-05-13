#pragma once

#include <QObject>
#include <QTimer>

class TaskScheduler : public QObject
{
    Q_OBJECT

public:
    explicit TaskScheduler(QObject *parent = nullptr);

    void startBigPauseTimer(int intervalMinutes);
    void startMiniPauseTimer(int intervalMinutes);
    void startWarningTimer(int minutesBefore);

    // Start timers for an exact millisecond delay without changing the
    // configured interval (used for postpone and inactivity-resume).
    void startBigPauseTimerMs(qint64 delayMs);
    void startMiniPauseTimerMs(qint64 delayMs);

    void resetBigPauseTimer();
    void resetMiniPauseTimer();

    void stopAll();

    // Remaining time in milliseconds (-1 if not active)
    qint64 bigPauseRemainingMs() const;
    qint64 miniPauseRemainingMs() const;

signals:
    void bigPauseDue();
    void miniPauseDue();
    void warningDue();

private:
    QTimer *m_bigPauseTimer{nullptr};
    QTimer *m_miniPauseTimer{nullptr};
    QTimer *m_warningTimer{nullptr};

    int m_bigPauseIntervalMs{0};
    int m_miniPauseIntervalMs{0};
};
