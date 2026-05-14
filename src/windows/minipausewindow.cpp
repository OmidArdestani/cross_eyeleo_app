#include "minipausewindow.h"
#include "settingsmanager.h"

#include <QApplication>
#include <QScreen>
#include <QFrame>
#include <QVBoxLayout>
#include <QMouseEvent>

MiniPauseWindow::MiniPauseWindow(SettingsManager *settings, QWidget *parent)
    : PauseWindowBase(settings, parent)
{
    init();

    setupUi();
}

void MiniPauseWindow::setupUi()
{
    // Full-screen dark-transparent overlay (same pattern as BigPauseWindow)
    setStyleSheet("background-color: rgba(0,0,0,170);");

    QVBoxLayout *outerLayout = new QVBoxLayout(this);
    outerLayout->setAlignment(Qt::AlignCenter);
    outerLayout->setContentsMargins(0, 0, 0, 0);

    // Centred card panel
    QFrame *card = new QFrame(this);
    card->setFixedSize(480, 340);
    card->setStyleSheet(
        "QFrame { background-color: rgba(30,30,45,230); border-radius: 16px; }");

    QVBoxLayout *layout = new QVBoxLayout(card);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(10);

    m_exerciseNameLabel = new QLabel(card);
    m_exerciseNameLabel->setAlignment(Qt::AlignCenter);
    QFont nameFont = m_exerciseNameLabel->font();
    nameFont.setPointSize(18);
    nameFont.setBold(true);
    m_exerciseNameLabel->setFont(nameFont);
    m_exerciseNameLabel->setStyleSheet("color: white;");

    m_animWidget = new ExerciseAnimationWidget(card);
    m_animWidget->setFixedSize(120, 120);

    m_instructionLabel = new QLabel(card);
    m_instructionLabel->setAlignment(Qt::AlignCenter);
    m_instructionLabel->setWordWrap(true);
    m_instructionLabel->setStyleSheet("color: rgba(255,255,255,200); font-size: 13px;");

    m_timerLabel = new QLabel("0:20", card);
    m_timerLabel->setAlignment(Qt::AlignCenter);
    QFont timerFont = m_timerLabel->font();
    timerFont.setPointSize(28);
    timerFont.setBold(true);
    m_timerLabel->setFont(timerFont);
    m_timerLabel->setStyleSheet("color: #4fc3f7;");

    layout->addWidget(m_exerciseNameLabel);
    layout->addWidget(m_animWidget, 0, Qt::AlignCenter);
    layout->addWidget(m_instructionLabel);
    layout->addWidget(m_timerLabel);

    outerLayout->addWidget(card);
}

void MiniPauseWindow::setExercise(ExerciseType type, int durationSeconds)
{
    Exercise ex = ExerciseManager::exerciseForType(type);
    m_exerciseNameLabel->setText(ex.name);
    m_instructionLabel->setText(ex.instruction);
    m_animWidget->setImage(ex.imagePath);
    m_totalSeconds = durationSeconds;
    m_remaining    = durationSeconds;
    onTick();
}

void MiniPauseWindow::showWithAnimation()
{
    if (m_settings->strictMode()) {
        showOnAllScreens();
    } else {
        showOnPrimaryScreen();
    }
    setWindowOpacity(0.0);
    QWidget::show();
    m_showAnim->start();
    m_countdown->start();
    m_animWidget->startAnimation();
}

void MiniPauseWindow::hideWithAnimation()
{
    m_countdown->stop();
    m_animWidget->stopAnimation();
    m_hideAnim->start();
}

void MiniPauseWindow::onTick()
{
    if (m_remaining <= 0) {
        m_countdown->stop();
        m_timerLabel->setText("0:00");
        emit completed();
        return;
    }

    int mins = m_remaining / 60;
    int secs = m_remaining % 60;
    m_timerLabel->setText(QString("%1:%2")
        .arg(mins)
        .arg(secs, 2, 10, QChar('0')));
    --m_remaining;
}

void MiniPauseWindow::onHideFinished()
{
    hide();
}

void MiniPauseWindow::mousePressEvent(QMouseEvent *event)
{
    if (!m_settings->strictMode()) {
        emit dismissed();
    }
    QWidget::mousePressEvent(event);
}
