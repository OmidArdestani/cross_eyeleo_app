#pragma once

#include <QWidget>
#include <QFrame>
#include <QPropertyAnimation>
#include <QTimer>
#include <QLabel>
#include <QPushButton>
#include <QtSvg/QSvgRenderer>

#include "exercises/exercisemanager.h"

class SettingsManager;

class MiniPauseWindow : public QWidget
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
    void onTick();
    void onHideFinished();

private:
    void setupUi();
    void showOnAllScreens();
    void updateImage(const QString &imagePath);

    SettingsManager    *m_settings{nullptr};

    QLabel             *m_exerciseNameLabel{nullptr};
    QLabel             *m_instructionLabel{nullptr};
    QLabel             *m_imageLabel{nullptr};
    QLabel             *m_timerLabel{nullptr};

    QPropertyAnimation *m_showAnim{nullptr};
    QPropertyAnimation *m_hideAnim{nullptr};
    QTimer             *m_countdown{nullptr};

    int m_totalSeconds{0};
    int m_remaining{0};
};
