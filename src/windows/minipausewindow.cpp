#include "minipausewindow.h"
#include "settingsmanager.h"

#include <QApplication>
#include <QScreen>
#include <QFrame>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QSvgRenderer>
#include <QPainter>

MiniPauseWindow::MiniPauseWindow(SettingsManager *settings, QWidget *parent)
    : QWidget(parent)
    , m_settings(settings)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);

    setupUi();

    m_showAnim = new QPropertyAnimation(this, "windowOpacity", this);
    m_showAnim->setDuration(500);
    m_showAnim->setStartValue(0.0);
    m_showAnim->setEndValue(1.0);

    m_hideAnim = new QPropertyAnimation(this, "windowOpacity", this);
    m_hideAnim->setDuration(500);
    m_hideAnim->setStartValue(1.0);
    m_hideAnim->setEndValue(0.0);
    connect(m_hideAnim, &QPropertyAnimation::finished, this, &MiniPauseWindow::onHideFinished);

    m_countdown = new QTimer(this);
    m_countdown->setInterval(1000);
    connect(m_countdown, &QTimer::timeout, this, &MiniPauseWindow::onTick);
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

    m_imageLabel = new QLabel(card);
    m_imageLabel->setAlignment(Qt::AlignCenter);
    m_imageLabel->setFixedSize(120, 120);

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
    layout->addWidget(m_imageLabel, 0, Qt::AlignCenter);
    layout->addWidget(m_instructionLabel);
    layout->addWidget(m_timerLabel);

    outerLayout->addWidget(card);
}

void MiniPauseWindow::showOnAllScreens()
{
    QRect totalGeometry;
    const auto screens = QApplication::screens();
    for (QScreen *screen : screens) {
        totalGeometry = totalGeometry.united(screen->geometry());
    }
    setGeometry(totalGeometry);
}

void MiniPauseWindow::updateImage(const QString &imagePath)
{
    QSvgRenderer renderer(imagePath);
    if (renderer.isValid()) {
        QPixmap px(120, 120);
        px.fill(Qt::transparent);
        QPainter painter(&px);
        renderer.render(&painter);
        m_imageLabel->setPixmap(px);
    } else {
        m_imageLabel->setText("👁");
    }
}

void MiniPauseWindow::setExercise(ExerciseType type, int durationSeconds)
{
    Exercise ex = ExerciseManager::exerciseForType(type);
    m_exerciseNameLabel->setText(ex.name);
    m_instructionLabel->setText(ex.instruction);
    updateImage(ex.imagePath);
    m_totalSeconds = durationSeconds;
    m_remaining    = durationSeconds;
    onTick();
}

void MiniPauseWindow::showWithAnimation()
{
    showOnAllScreens();
    setWindowOpacity(0.0);
    QWidget::show();
    m_showAnim->start();
    m_countdown->start();
}

void MiniPauseWindow::hideWithAnimation()
{
    m_countdown->stop();
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
