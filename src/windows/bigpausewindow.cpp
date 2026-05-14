#include "bigpausewindow.h"
#include "settingsmanager.h"

#include <QApplication>
#include <QScreen>
#include <QVBoxLayout>
#include <QHBoxLayout>

BigPauseWindow::BigPauseWindow(SettingsManager *settings, QWidget *parent)
    : PauseWindowBase(settings, parent)
{
    init();

    setupUi();
}

void BigPauseWindow::setupUi()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(20);
    layout->setContentsMargins(60, 60, 60, 60);

    m_titleLabel = new QLabel(tr("Time for a long break!"), this);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont = m_titleLabel->font();
    titleFont.setPointSize(28);
    titleFont.setBold(true);
    m_titleLabel->setFont(titleFont);
    m_titleLabel->setStyleSheet("color: white;");

    m_timerLabel = new QLabel("05:00", this);
    m_timerLabel->setAlignment(Qt::AlignCenter);
    QFont timerFont = m_timerLabel->font();
    timerFont.setPointSize(64);
    timerFont.setBold(true);
    m_timerLabel->setFont(timerFont);
    m_timerLabel->setStyleSheet("color: #4fc3f7;");

    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(100);
    m_progressBar->setTextVisible(false);
    m_progressBar->setFixedHeight(8);
    m_progressBar->setStyleSheet(
        "QProgressBar { background: rgba(255,255,255,60); border-radius: 4px; }"
        "QProgressBar::chunk { background: #4fc3f7; border-radius: 4px; }");

    m_instructionLabel = new QLabel(tr("Relax your eyes and take a break from the screen."), this);
    m_instructionLabel->setAlignment(Qt::AlignCenter);
    QFont instrFont = m_instructionLabel->font();
    instrFont.setPointSize(14);
    m_instructionLabel->setFont(instrFont);
    m_instructionLabel->setStyleSheet("color: rgba(255,255,255,200);");

    m_skipButton = new QPushButton(tr("Skip Break"), this);
    m_skipButton->setStyleSheet(
        "QPushButton { background: rgba(255,255,255,40); color: white; border: 1px solid rgba(255,255,255,100);"
        " border-radius: 8px; padding: 10px 30px; font-size: 14px; }"
        "QPushButton:hover { background: rgba(255,255,255,70); }");
    connect(m_skipButton, &QPushButton::clicked, this, &BigPauseWindow::skipClicked);

    layout->addStretch();
    layout->addWidget(m_titleLabel);
    layout->addWidget(m_timerLabel);
    layout->addWidget(m_progressBar);
    layout->addWidget(m_instructionLabel);
    layout->addSpacing(20);
    layout->addWidget(m_skipButton, 0, Qt::AlignCenter);
    layout->addStretch();

    setStyleSheet("background-color: rgba(0,0,0,180);");
}

void BigPauseWindow::startCountdown(int durationSeconds)
{
    m_totalSeconds = durationSeconds;
    m_remaining    = durationSeconds;
    onTick();
}

void BigPauseWindow::showWithAnimation()
{
    m_skipButton->setVisible(!m_settings->strictMode());
    showOnAllScreens();
    setWindowOpacity(0.0);
    QWidget::show();
    m_showAnim->start();
    m_countdown->start();
}

void BigPauseWindow::hideWithAnimation()
{
    m_countdown->stop();
    m_hideAnim->start();
}

void BigPauseWindow::onTick()
{
    if (m_remaining <= 0) {
        m_countdown->stop();
        m_timerLabel->setText("00:00");
        m_progressBar->setValue(0);
        emit breakCompleted();
        return;
    }

    int mins = m_remaining / 60;
    int secs = m_remaining % 60;
    m_timerLabel->setText(QString("%1:%2")
        .arg(mins, 2, 10, QChar('0'))
        .arg(secs, 2, 10, QChar('0')));

    int pct = (m_totalSeconds > 0)
        ? static_cast<int>((static_cast<double>(m_remaining) / m_totalSeconds) * 100)
        : 0;
    m_progressBar->setValue(pct);

    --m_remaining;
}

void BigPauseWindow::onHideFinished()
{
    hide();
}
