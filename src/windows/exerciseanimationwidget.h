#pragma once

#include <QWidget>
#include <QTimer>
#include "exercises/exercisemanager.h"

/**
 * @brief Draws an exercise-specific animation using QPainter at ~30 fps.
 *
 * Each ExerciseType has its own drawing function that conveys the actual
 * movement the user should perform (e.g. eyelid closing/opening for BLINK,
 * iris tracing a circle for ROLL, etc.).
 */
class ExerciseAnimationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ExerciseAnimationWidget(QWidget *parent = nullptr);

    void setExercise(ExerciseType type);
    void startAnimation();
    void stopAnimation();

    QSize sizeHint() const override { return QSize(120, 120); }

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onTimer();

private:
    // Per-exercise drawing functions
    void drawBlink(QPainter &p, const QRectF &r);
    void drawRoll(QPainter &p, const QRectF &r);
    void drawLookVert(QPainter &p, const QRectF &r);
    void drawLookHorz(QPainter &p, const QRectF &r);
    void drawCloseTightly(QPainter &p, const QRectF &r);
    void drawWindow(QPainter &p, const QRectF &r);
    void drawStretch(QPainter &p, const QRectF &r);

    // Shared helper: draw one eye with optional iris offset and eyelid close amount.
    void drawEye(QPainter &p, QPointF center, qreal rx, qreal ry,
                 qreal irisOx = 0.0, qreal irisOy = 0.0, qreal lidClose = 0.0);

    ExerciseType m_type{ExerciseType::BLINK};
    QTimer      *m_timer{nullptr};
    qreal        m_t{0.0};   ///< Animation progress in [0, 1), cycles continuously.
};
