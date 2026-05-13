#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QPropertyAnimation>
#include <QTimer>

class BeforePauseWindow : public QWidget
{
    Q_OBJECT

public:
    explicit BeforePauseWindow(QWidget *parent = nullptr);

    void startCountdown(int seconds = 30);

signals:
    void readyClicked();
    void postponeClicked();

private slots:
    void onTick();

private:
    void setupUi();
    void centerOnScreen();

    QLabel      *m_messageLabel{nullptr};
    QLabel      *m_countdownLabel{nullptr};
    QPushButton *m_readyButton{nullptr};
    QPushButton *m_postponeButton{nullptr};
    QTimer      *m_timer{nullptr};

    int m_remaining{30};
};
