#pragma once

#include <QObject>
#include <QTimer>

#if defined(HAVE_XSCRNSAVER)
#include <X11/extensions/scrnsaver.h>
#endif

class ActivityMonitor : public QObject
{
    Q_OBJECT

public:
    explicit ActivityMonitor(QObject *parent = nullptr);
    ~ActivityMonitor() override;

    void start();
    void stop();

    qint64 inactivityThreshold() const { return m_threshold; }
    void setInactivityThreshold(qint64 ms) { m_threshold = ms; }

    qint64 getIdleTimeMs();

signals:
    void userInactive();
    void userReturned();

private slots:
    void onPollTimer();

private:
    QTimer *m_pollTimer{nullptr};
    qint64  m_threshold{300000}; // 5 minutes default
    bool    m_wasInactive{false};

#if defined(HAVE_XSCRNSAVER)
    Display *m_display{nullptr};
#endif
};
