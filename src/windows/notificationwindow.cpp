#include "notificationwindow.h"
#include "settingsmanager.h"

#include <QApplication>
#include <QScreen>
#include <QVBoxLayout>
#include <QMouseEvent>

NotificationWindow::NotificationWindow(SettingsManager *settings, QWidget *parent)
    : QWidget(parent)
    , m_settings(settings)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(300, 80);

    setupUi();

    m_showAnim = new QPropertyAnimation(this, "windowOpacity", this);
    m_showAnim->setDuration(300);
    m_showAnim->setStartValue(0.0);
    m_showAnim->setEndValue(1.0);

    m_hideAnim = new QPropertyAnimation(this, "windowOpacity", this);
    m_hideAnim->setDuration(300);
    m_hideAnim->setStartValue(1.0);
    m_hideAnim->setEndValue(0.0);
    connect(m_hideAnim, &QPropertyAnimation::finished, this, &NotificationWindow::onHideFinished);

    m_autoHideTimer = new QTimer(this);
    m_autoHideTimer->setSingleShot(true);
    m_autoHideTimer->setInterval(10000);
    connect(m_autoHideTimer, &QTimer::timeout, this, &NotificationWindow::onAutoHide);
}

void NotificationWindow::setupUi()
{
    setStyleSheet(
        "background-color: rgba(38,50,56,230); "
        "border-radius: 10px; "
        "border: 1px solid rgba(255,255,255,40);");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(16, 12, 16, 12);

    m_messageLabel = new QLabel(this);
    m_messageLabel->setWordWrap(true);
    m_messageLabel->setAlignment(Qt::AlignCenter);
    m_messageLabel->setStyleSheet("color: white; font-size: 13px;");

    layout->addWidget(m_messageLabel);
}

void NotificationWindow::positionWindow()
{
    QScreen *screen = QApplication::primaryScreen();
    QRect sg = screen->availableGeometry();
    int x = sg.right()  - width()  - 20;
    int y = sg.bottom() - height() - 20;
    move(x, y);
}

void NotificationWindow::showNotification(const QString &message)
{
    m_messageLabel->setText(message);
    positionWindow();
    setWindowOpacity(0.0);
    QWidget::show();
    m_showAnim->start();
    m_autoHideTimer->start();
}

void NotificationWindow::mousePressEvent(QMouseEvent *event)
{
    if (!m_settings || m_settings->canCloseNotifications()) {
        onAutoHide();
    }
    QWidget::mousePressEvent(event);
}

void NotificationWindow::onAutoHide()
{
    m_autoHideTimer->stop();
    m_hideAnim->start();
}

void NotificationWindow::onHideFinished()
{
    hide();
}
