#include "activitymonitor.h"

#if defined(Q_OS_WIN)
#  include <windows.h>
#elif defined(Q_OS_MAC)
#  include <CoreGraphics/CGEventSource.h>
#elif defined(HAVE_XSCRNSAVER)
#  include <X11/Xlib.h>
#  include <X11/extensions/scrnsaver.h>
#endif

ActivityMonitor::ActivityMonitor(QObject *parent)
    : QObject(parent)
{
    m_pollTimer = new QTimer(this);
    m_pollTimer->setInterval(30000); // poll every 30 seconds
    connect(m_pollTimer, &QTimer::timeout, this, &ActivityMonitor::onPollTimer);

#if defined(HAVE_XSCRNSAVER)
    m_display = XOpenDisplay(nullptr);
#endif
}

ActivityMonitor::~ActivityMonitor()
{
#if defined(HAVE_XSCRNSAVER)
    if (m_display) {
        XCloseDisplay(m_display);
        m_display = nullptr;
    }
#endif
}

void ActivityMonitor::start()
{
    m_wasInactive = false;
    m_pollTimer->start();
}

void ActivityMonitor::stop()
{
    m_pollTimer->stop();
}

qint64 ActivityMonitor::getIdleTimeMs()
{
#if defined(Q_OS_WIN)
    LASTINPUTINFO lii;
    lii.cbSize = sizeof(lii);
    if (GetLastInputInfo(&lii)) {
        return static_cast<qint64>(GetTickCount() - lii.dwTime);
    }
    return 0;
#elif defined(Q_OS_MAC)
    return static_cast<qint64>(
        CGEventSourceSecondsSinceLastEventType(
            kCGEventSourceStateCombinedSessionState,
            kCGAnyInputEventType) * 1000);
#elif defined(HAVE_XSCRNSAVER)
    if (m_display) {
        XScreenSaverInfo *info = XScreenSaverAllocInfo();
        XScreenSaverQueryInfo(m_display, DefaultRootWindow(m_display), info);
        qint64 idle = static_cast<qint64>(info->idle);
        XFree(info);
        return idle;
    }
    return 0;
#else
    return 0;
#endif
}

void ActivityMonitor::onPollTimer()
{
    qint64 idle = getIdleTimeMs();
    if (idle >= m_threshold) {
        if (!m_wasInactive) {
            m_wasInactive = true;
            emit userInactive();
        }
    } else {
        if (m_wasInactive) {
            m_wasInactive = false;
            emit userReturned();
        }
    }
}
