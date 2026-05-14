#include "exerciseanimationwidget.h"

#include <QDebug>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QRadialGradient>
#include <cmath>

// M_PI is not guaranteed by the C++ standard; provide a portable fallback.
#ifndef M_PI
static constexpr double kPi = 3.14159265358979323846;
#else
static constexpr double kPi = M_PI;
#endif

ExerciseAnimationWidget::ExerciseAnimationWidget(QWidget *parent)
    : QWidget(parent)
{
    // 30 fps animation timer.
    m_timer = new QTimer(this);
    m_timer->setInterval(1000 / 30);
    connect(m_timer, &QTimer::timeout, this, &ExerciseAnimationWidget::onTimer);
}

void ExerciseAnimationWidget::setExercise(ExerciseType type)
{
    m_type = type;
    m_t = 0.0;
    update();
}

void ExerciseAnimationWidget::startAnimation()
{
    m_t = 0.0;
    m_timer->start();
}

void ExerciseAnimationWidget::stopAnimation()
{
    m_timer->stop();
    m_t = 0.0;
    update();
}

void ExerciseAnimationWidget::onTimer()
{
    // Advance by one frame of a 2-second cycle.
    m_t += 1.0 / (30.0 * 2.0);
    if (m_t >= 1.0) m_t -= 1.0;
    update();
}

// ---------------------------------------------------------------------------
// Shared eye-drawing helper
// ---------------------------------------------------------------------------
void ExerciseAnimationWidget::drawEye(QPainter &p, QPointF c,
                                      qreal rx, qreal ry,
                                      qreal irisOx, qreal irisOy,
                                      qreal lidClose)
{
    const QRectF eyeRect(c.x() - rx, c.y() - ry, 2 * rx, 2 * ry);
    QPainterPath eyePath;
    eyePath.addEllipse(eyeRect);

    // Eye white
    p.setBrush(QColor(248, 248, 248));
    p.setPen(QPen(QColor(55, 55, 55), 1.6));
    p.drawEllipse(eyeRect);

    // Iris + pupil, clipped to eye white
    const qreal ir = qMin(rx, ry) * 0.86;
    const QPointF ic(c.x() + irisOx, c.y() + irisOy);
    p.save();
    p.setClipPath(eyePath);
    p.setPen(Qt::NoPen);

    QRadialGradient irisGrad(ic, ir);
    irisGrad.setColorAt(0.0, QColor(55, 155, 240));
    irisGrad.setColorAt(1.0, QColor(18, 90, 170));
    p.setBrush(irisGrad);
    p.drawEllipse(ic, ir, ir);

    p.setBrush(QColor(20, 20, 20));
    p.drawEllipse(ic, ir * 0.48, ir * 0.48);

    p.setBrush(QColor(255, 255, 255, 210));
    p.drawEllipse(QPointF(ic.x() - ir * 0.26, ic.y() - ir * 0.28),
                  ir * 0.22, ir * 0.22);
    p.restore();

    // Eyelid overlay when closing (skin-tone fill from top and bottom)
    if (lidClose > 0.001) {
        const qreal topY = c.y() - ry;
        const qreal botY = c.y() + ry;
        const qreal upperBottom = topY + 2.0 * ry * lidClose;
        const qreal lowerTop    = botY - 2.0 * ry * lidClose * 0.55;

        p.setPen(Qt::NoPen);
        p.setBrush(QColor(210, 165, 130));

        // Upper eyelid
        QPainterPath topClip;
        topClip.addRect(QRectF(c.x() - rx - 1, topY - 1,
                               2 * rx + 2, (upperBottom - topY) + 1));
        p.drawPath(eyePath.intersected(topClip));

        // Lower eyelid
        if (lowerTop < botY) {
            QPainterPath botClip;
            botClip.addRect(QRectF(c.x() - rx - 1, lowerTop,
                                   2 * rx + 2, botY - lowerTop + 1));
            p.drawPath(eyePath.intersected(botClip));
        }
    }

    // Eye outline on top of everything
    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(QColor(55, 55, 55), 1.6));
    p.drawEllipse(eyeRect);
}

// ---------------------------------------------------------------------------
// Exercise-specific drawing functions
// ---------------------------------------------------------------------------

void ExerciseAnimationWidget::drawBlink(QPainter &p, const QRectF &r)
{
    // Two rapid blinks per 2-second cycle.
    // Each blink occupies 0.30 units: 0.10 closing, 0.05 fully closed, 0.15 opening.
    auto blinkLid = [](qreal t, qreal tStart) -> qreal {
        const qreal tClose = tStart + 0.10;
        const qreal tOpen  = tStart + 0.15;
        const qreal tEnd   = tStart + 0.30;
        if (t < tStart || t >= tEnd) return 0.0;
        if (t < tClose) return (t - tStart) / (tClose - tStart);
        if (t < tOpen)  return 1.0;
        return 1.0 - (t - tOpen) / (tEnd - tOpen);
    };

    const qreal lid = qMax(blinkLid(m_t, 0.10), blinkLid(m_t, 0.60));
    const qreal rx  = r.width()  * 0.40;
    const qreal ry  = r.height() * 0.22;
    drawEye(p, r.center(), rx, ry, 0.0, 0.0, lid);
}

void ExerciseAnimationWidget::drawRoll(QPainter &p, const QRectF &r)
{
    const qreal rx     = r.width()  * 0.40;
    const qreal ry     = r.height() * 0.22;
    const qreal orbitR = qMin(rx, ry) * 0.46;
    const qreal angle  = 2.0 * kPi * m_t;
    drawEye(p, r.center(), rx, ry,
            orbitR * std::cos(angle),
            orbitR * std::sin(angle));
}

void ExerciseAnimationWidget::drawLookVert(QPainter &p, const QRectF &r)
{
    const qreal rx     = r.width()  * 0.40;
    const qreal ry     = r.height() * 0.22;
    const qreal travel = ry * 0.55;
    const qreal ioy    = travel * std::sin(2.0 * kPi * m_t);
    drawEye(p, r.center(), rx, ry, 0.0, ioy);

    // Direction arrows
    const QPointF c = r.center();
    p.setPen(QPen(QColor(229, 57, 53), 2.2, Qt::SolidLine, Qt::RoundCap));
    const qreal ax   = c.x();
    const qreal topY = c.y() - ry - 14;
    const qreal botY = c.y() + ry + 14;

    p.drawLine(QPointF(ax, topY + 9), QPointF(ax, topY));
    p.drawLine(QPointF(ax - 5, topY + 6), QPointF(ax, topY));
    p.drawLine(QPointF(ax + 5, topY + 6), QPointF(ax, topY));

    p.drawLine(QPointF(ax, botY - 9), QPointF(ax, botY));
    p.drawLine(QPointF(ax - 5, botY - 6), QPointF(ax, botY));
    p.drawLine(QPointF(ax + 5, botY - 6), QPointF(ax, botY));
}

void ExerciseAnimationWidget::drawLookHorz(QPainter &p, const QRectF &r)
{
    const qreal rx     = r.width()  * 0.40;
    const qreal ry     = r.height() * 0.22;
    const qreal travel = rx * 0.46;
    const qreal iox    = travel * std::sin(2.0 * kPi * m_t);
    drawEye(p, r.center(), rx, ry, iox, 0.0);

    // Direction arrows
    const QPointF c  = r.center();
    p.setPen(QPen(QColor(229, 57, 53), 2.2, Qt::SolidLine, Qt::RoundCap));
    const qreal ay     = c.y();
    const qreal leftX  = c.x() - rx - 14;
    const qreal rightX = c.x() + rx + 14;

    p.drawLine(QPointF(leftX + 9, ay), QPointF(leftX, ay));
    p.drawLine(QPointF(leftX + 6, ay - 5), QPointF(leftX, ay));
    p.drawLine(QPointF(leftX + 6, ay + 5), QPointF(leftX, ay));

    p.drawLine(QPointF(rightX - 9, ay), QPointF(rightX, ay));
    p.drawLine(QPointF(rightX - 6, ay - 5), QPointF(rightX, ay));
    p.drawLine(QPointF(rightX - 6, ay + 5), QPointF(rightX, ay));
}

void ExerciseAnimationWidget::drawCloseTightly(QPainter &p, const QRectF &r)
{
    // Open → slow close → hold shut → slow open → open
    qreal lid;
    if      (m_t < 0.20) lid = 0.0;
    else if (m_t < 0.40) lid = (m_t - 0.20) / 0.20;
    else if (m_t < 0.60) lid = 1.0;
    else if (m_t < 0.80) lid = 1.0 - (m_t - 0.60) / 0.20;
    else                  lid = 0.0;

    const qreal rx = r.width()  * 0.40;
    const qreal ry = r.height() * 0.22;
    drawEye(p, r.center(), rx, ry, 0.0, 0.0, lid);
}

void ExerciseAnimationWidget::drawWindow(QPainter &p, const QRectF &r)
{
    // Alternates "near focus" (first half) and "far focus" (second half).
    const QPointF c      = r.center();
    const bool    near   = (m_t < 0.5);
    // Pulse: 0→1→0 within each half-cycle
    const qreal   pulse  = std::sin(2.0 * kPi * m_t);

    // Near object (small square in centre)
    const qreal nearSz = 14.0 + (near ? pulse * 3.0 : 0.0);
    p.setBrush(near ? QColor(79, 195, 247) : QColor(79, 195, 247, 80));
    p.setPen(QPen(near ? QColor(25, 118, 210) : QColor(25, 118, 210, 80), 2.0));
    p.drawRect(QRectF(c.x() - nearSz / 2, c.y() - nearSz / 2, nearSz, nearSz));

    // Far object (large ring)
    const qreal farR = r.width() * 0.36;
    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(!near ? QColor(79, 195, 247) : QColor(79, 195, 247, 80),
                  !near ? 3.0 : 1.2));
    p.drawEllipse(c, farR, farR);

    // Travelling focus ring between near and far
    const qreal phase    = m_t < 0.5 ? m_t * 2.0 : (m_t - 0.5) * 2.0;
    const qreal ringR    = near ? nearSz * 0.9 + (farR - nearSz * 0.9) * phase
                                : farR - (farR - nearSz * 0.9) * phase;
    const int   alpha    = static_cast<int>(255 * std::abs(std::sin(kPi * m_t)));
    p.setPen(QPen(QColor(79, 195, 247, alpha), 2.0));
    if (ringR > 2 && ringR < farR + 4)
        p.drawEllipse(c, ringR, ringR);

    // Label
    p.setPen(QColor(200, 200, 200));
    QFont f = p.font(); f.setPointSize(7); p.setFont(f);
    p.drawText(QRectF(c.x() - 30, c.y() + r.height() * 0.36, 60, 14),
               Qt::AlignCenter, near ? tr("Near") : tr("Far (~6 m)"));
}

void ExerciseAnimationWidget::drawStretch(QPainter &p, const QRectF &r)
{
    // Head + neck tilting left and right.
    const QPointF c    = r.center();
    const qreal   tilt = 22.0 * std::sin(2.0 * kPi * m_t);  // degrees

    p.save();
    p.translate(c);
    p.rotate(tilt);

    // Neck
    p.setPen(QPen(QColor(200, 160, 120), 7, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(QPointF(0, 0), QPointF(0, r.height() * 0.20));

    // Head (slightly oval)
    const qreal hR = r.width() * 0.18;
    p.setBrush(QColor(225, 185, 145));
    p.setPen(QPen(QColor(160, 115, 80), 1.8));
    p.drawEllipse(QPointF(0, -hR * 0.5), hR, hR * 1.1);

    // Simple eye dots
    p.setBrush(QColor(50, 50, 50));
    p.setPen(Qt::NoPen);
    p.drawEllipse(QPointF(-hR * 0.30, -hR * 0.28), hR * 0.08, hR * 0.08);
    p.drawEllipse(QPointF( hR * 0.30, -hR * 0.28), hR * 0.08, hR * 0.08);

    p.restore();

    // Direction arrow (shows which way to tilt)
    const qreal  side   = (tilt > 0) ? 1.0 : -1.0;
    const qreal  arrowX = c.x() + side * r.width() * 0.40;
    const qreal  aTop   = c.y() - r.height() * 0.12;
    const qreal  aBot   = c.y() + r.height() * 0.12;
    p.setPen(QPen(QColor(229, 57, 53, 200), 2.2, Qt::SolidLine, Qt::RoundCap));
    p.drawLine(QPointF(arrowX, aTop), QPointF(arrowX, aBot));
    // Arrow tip points downward on the side toward which the head tilts.
    p.drawLine(QPointF(arrowX - 5, aBot - 7), QPointF(arrowX, aBot));
    p.drawLine(QPointF(arrowX + 5, aBot - 7), QPointF(arrowX, aBot));
}

// ---------------------------------------------------------------------------
void ExerciseAnimationWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform);

    const QRectF r = QRectF(rect()).adjusted(4, 4, -4, -4);

    switch (m_type) {
        case ExerciseType::BLINK:         drawBlink(p, r);        break;
        case ExerciseType::ROLL:          drawRoll(p, r);         break;
        case ExerciseType::LOOK_VERT:     drawLookVert(p, r);     break;
        case ExerciseType::LOOK_HORZ:     drawLookHorz(p, r);     break;
        case ExerciseType::CLOSE_TIGHTLY: drawCloseTightly(p, r); break;
        case ExerciseType::WINDOW:        drawWindow(p, r);       break;
        case ExerciseType::STRETCH:       drawStretch(p, r);      break;
    }
}
