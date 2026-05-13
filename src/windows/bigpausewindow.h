#pragma once

#include <QWidget>
#include <QPropertyAnimation>
#include <QTimer>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>

class SettingsManager;

class BigPauseWindow : public QWidget
{
    Q_OBJECT

public:
    explicit BigPauseWindow(SettingsManager *settings, QWidget *parent = nullptr);

    void startCountdown(int durationSeconds);
    void showWithAnimation();
    void hideWithAnimation();

signals:
    void skipClicked();
    void breakCompleted();

private slots:
    void onTick();
    void onHideFinished();

private:
    void setupUi();
    void showOnAllScreens();

    SettingsManager    *m_settings{nullptr};

    QLabel             *m_titleLabel{nullptr};
    QLabel             *m_timerLabel{nullptr};
    QProgressBar       *m_progressBar{nullptr};
    QLabel             *m_instructionLabel{nullptr};
    QPushButton        *m_skipButton{nullptr};

    QPropertyAnimation *m_showAnim{nullptr};
    QPropertyAnimation *m_hideAnim{nullptr};
    QTimer             *m_countdown{nullptr};

    int m_totalSeconds{0};
    int m_remaining{0};
};
