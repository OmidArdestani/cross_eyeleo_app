#pragma once

#include <QWidget>
#include <QFrame>
#include <QPropertyAnimation>
#include <QTimer>
#include <QLabel>
#include <QPushButton>
#include <QtSvg/QSvgRenderer>

#include "exercises/exercisemanager.h"

#include "pausewindowbase.hpp"

class SettingsManager;

class MiniPauseWindow : public PauseWindowBase
{
    Q_OBJECT

public:
    explicit MiniPauseWindow(SettingsManager *settings, QWidget *parent = nullptr);

    void setExercise(ExerciseType type, int durationSeconds);
    void showWithAnimation();
    void hideWithAnimation();

signals:
    void dismissed();
    void completed();

protected:
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void onTick() override;
    void onHideFinished() override;

private:
    void setupUi();
    void updateImage(const QString &imagePath);

    QLabel             *m_exerciseNameLabel{nullptr};
    QLabel             *m_instructionLabel{nullptr};
    QLabel             *m_imageLabel{nullptr};
    QLabel             *m_timerLabel{nullptr};

    int m_totalSeconds{0};
    int m_remaining{0};
};
