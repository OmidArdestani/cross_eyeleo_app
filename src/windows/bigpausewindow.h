#pragma once

#include <QWidget>
#include <QPropertyAnimation>
#include <QTimer>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include "pausewindowbase.hpp"

class BigPauseWindow : public PauseWindowBase
{
    Q_OBJECT

public:
    explicit BigPauseWindow(SettingsManager *settings, QWidget *parent = nullptr);

    void startCountdown(int durationSeconds);
    void showWithAnimation();
    void hideWithAnimation();

private slots:
    void onTick() override;
    void onHideFinished() override;

private:
    void setupUi();

    QLabel             *m_titleLabel{nullptr};
    QLabel             *m_timerLabel{nullptr};
    QProgressBar       *m_progressBar{nullptr};
    QLabel             *m_instructionLabel{nullptr};
    QPushButton        *m_skipButton{nullptr};

    int m_totalSeconds{0};
    int m_remaining{0};
};
