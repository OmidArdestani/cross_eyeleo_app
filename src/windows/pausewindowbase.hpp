
#pragma once

#include <QApplication>
#include <QPropertyAnimation>
#include <QScreen>
#include <QTimer>
#include <QWidget>


class SettingsManager;

class PauseWindowBase : public QWidget
{
    Q_OBJECT
public:
    explicit PauseWindowBase(SettingsManager *settings, QWidget *parent = nullptr)
        : QWidget{parent}
        , m_settings(settings)
    {
    }

protected:
    virtual void showOnPrimaryScreen()
    {
        QScreen *screen = QApplication::primaryScreen();
        if (!screen) {
            return;
        }

        const QRect screenGeometry = screen->availableGeometry();
        QSize targetSize = size();
        if (!targetSize.isValid() || targetSize.isEmpty()) {
            targetSize = sizeHint();
        }

        targetSize = targetSize.boundedTo(screenGeometry.size());

        QRect centeredGeometry(QPoint(0, 0), targetSize);
        centeredGeometry.moveCenter(screenGeometry.center());
        setGeometry(centeredGeometry);
    }

    // Covers the virtual desktop formed by all connected screens.
    // Used in strict mode to block access to every monitor.
    virtual void showOnScreen(QScreen *screen)
    {
        if (!screen) {
            showOnPrimaryScreen();
            return;
        }
        // Use the full geometry (including taskbar/dock) so nothing peeks through.
        setGeometry(screen->geometry());
    }

    virtual void setupOpacityAnim(const int& duration)
    {
        m_showAnim = new QPropertyAnimation(this, "windowOpacity", this);
        m_showAnim->setDuration(duration);
        m_showAnim->setStartValue(0.0);
        m_showAnim->setEndValue(1.0);

        m_hideAnim = new QPropertyAnimation(this, "windowOpacity", this);
        m_hideAnim->setDuration(duration);
        m_hideAnim->setStartValue(1.0);
        m_hideAnim->setEndValue(0.0);
        connect(m_hideAnim, &QPropertyAnimation::finished, this, &PauseWindowBase::onHideFinished);
    }

protected slots:
    virtual void onHideFinished(){}
    virtual void onTick(){}

protected:
    virtual void init()
    {
        setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
        setAttribute(Qt::WA_TranslucentBackground);

        setupOpacityAnim(1000);

        m_countdown = new QTimer(this);
        m_countdown->setInterval(1000);
        connect(m_countdown, &QTimer::timeout, this, &PauseWindowBase::onTick);
    }

signals:
    void skipClicked();
    void breakCompleted();

protected:
    SettingsManager    *m_settings{nullptr};
    QPropertyAnimation *m_showAnim{nullptr};
    QPropertyAnimation *m_hideAnim{nullptr};
    QTimer             *m_countdown{nullptr};
};
