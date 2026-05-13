#pragma once

#include <QObject>
#include <QTimer>

// X11 headers must NOT be included here because they define macros (None, Bool,
// True, False, Status, …) that break Qt and C++ headers included afterwards in
// the same compilation unit (in particular mocs_compilation.cpp, where
// moc_activitymonitor.cpp is the first file processed alphabetically).
// All X11/platform-specific includes live in activitymonitor.cpp only.

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
    // Stored as void* to avoid pulling <X11/Xlib.h> into this header.
    // activitymonitor.cpp casts it to Display* internally.
    void *m_display{nullptr};
#endif
};
