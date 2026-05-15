#include "beforepausewindow.h"

#include <QApplication>
#include <QScreen>
#include <QVBoxLayout>
#include <QHBoxLayout>

BeforePauseWindow::BeforePauseWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint);
    setWindowTitle(tr("Break Reminder"));
    setFixedSize(360, 180);

    setupUi();

    m_timer = new QTimer(this);
    m_timer->setInterval(1000);
    connect(m_timer, &QTimer::timeout, this, &BeforePauseWindow::onTick);

    // Stop the countdown when either button is clicked so the timer cannot
    // fire readyClicked() after the user has already acted on the window
    // (e.g. chosen to postpone).
    connect(m_readyButton,    &QPushButton::clicked, m_timer, &QTimer::stop);
    connect(m_postponeButton, &QPushButton::clicked, m_timer, &QTimer::stop);
}

void BeforePauseWindow::setupUi()
{
    setStyleSheet("background-color: #263238; color: white;");

    QVBoxLayout *vl = new QVBoxLayout(this);
    vl->setContentsMargins(24, 24, 24, 24);
    vl->setSpacing(12);

    m_messageLabel = new QLabel(tr("Long break is about to start"), this);
    m_messageLabel->setAlignment(Qt::AlignCenter);
    QFont f = m_messageLabel->font();
    f.setPointSize(13);
    f.setBold(true);
    m_messageLabel->setFont(f);
    m_messageLabel->setStyleSheet("color: white;");

    m_countdownLabel = new QLabel(tr("Starting in 30 seconds..."), this);
    m_countdownLabel->setAlignment(Qt::AlignCenter);
    m_countdownLabel->setStyleSheet("color: #4fc3f7; font-size: 14px;");

    QHBoxLayout *hl = new QHBoxLayout();
    m_readyButton    = new QPushButton(tr("I'm Ready"), this);
    m_postponeButton = new QPushButton(tr("Postpone (5 min)"), this);

    QString btnStyle =
        "QPushButton { background: rgba(255,255,255,30); color: white; border: 1px solid rgba(255,255,255,80);"
        " border-radius: 6px; padding: 8px 20px; }"
        "QPushButton:hover { background: rgba(255,255,255,60); }";
    m_readyButton->setStyleSheet(btnStyle);
    m_postponeButton->setStyleSheet(btnStyle);

    connect(m_readyButton,    &QPushButton::clicked, this, &BeforePauseWindow::readyClicked);
    connect(m_postponeButton, &QPushButton::clicked, this, &BeforePauseWindow::postponeClicked);

    hl->addWidget(m_readyButton);
    hl->addWidget(m_postponeButton);

    vl->addWidget(m_messageLabel);
    vl->addWidget(m_countdownLabel);
    vl->addStretch();
    vl->addLayout(hl);
}

void BeforePauseWindow::centerOnScreen()
{
    QScreen *screen = QApplication::primaryScreen();
    QRect sg = screen->availableGeometry();
    move(sg.center().x() - width() / 2,
         sg.center().y() - height() / 2);
}

void BeforePauseWindow::startCountdown(int seconds)
{
    m_remaining = seconds;
    m_countdownLabel->setText(tr("Starting in %1 seconds...").arg(m_remaining));
    centerOnScreen();
    m_timer->start();
}

void BeforePauseWindow::onTick()
{
    --m_remaining;
    m_countdownLabel->setText(tr("Starting in %1 seconds...").arg(m_remaining));
    if (m_remaining <= 0) {
        m_timer->stop();
        emit readyClicked();
    }
}
