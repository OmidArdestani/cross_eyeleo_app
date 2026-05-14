#include "exerciseanimationwidget.h"

#include <QDebug>
#include <QEasingCurve>
#include <QPaintEvent>
#include <QPainter>

ExerciseAnimationWidget::ExerciseAnimationWidget(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_OpaquePaintEvent, false);

    m_renderer = new QSvgRenderer(this);

    // Breathing animation: gently scale up then back down, looping forever.
    auto *scaleUp = new QPropertyAnimation(this, "animScale", this);
    scaleUp->setDuration(900);
    scaleUp->setStartValue(1.0);
    scaleUp->setEndValue(1.15);
    scaleUp->setEasingCurve(QEasingCurve::InOutSine);

    auto *scaleDown = new QPropertyAnimation(this, "animScale", this);
    scaleDown->setDuration(900);
    scaleDown->setStartValue(1.15);
    scaleDown->setEndValue(1.0);
    scaleDown->setEasingCurve(QEasingCurve::InOutSine);

    m_animGroup = new QSequentialAnimationGroup(this);
    m_animGroup->addAnimation(scaleUp);
    m_animGroup->addAnimation(scaleDown);
    m_animGroup->setLoopCount(-1);  // loop forever
}

void ExerciseAnimationWidget::setImage(const QString &svgPath)
{
    m_renderer->load(svgPath);
    if (!m_renderer->isValid()) {
        qWarning("ExerciseAnimationWidget: failed to load SVG '%s'",
                 qUtf8Printable(svgPath));
    }
    update();
}

void ExerciseAnimationWidget::startAnimation()
{
    m_animGroup->start();
}

void ExerciseAnimationWidget::stopAnimation()
{
    m_animGroup->stop();
    m_scale = 1.0;
    update();
}

void ExerciseAnimationWidget::setAnimScale(qreal scale)
{
    m_scale = scale;
    update();
}

void ExerciseAnimationWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    if (!m_renderer->isValid()) return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    const qreal w = width();
    const qreal h = height();
    const qreal scaledW = w * m_scale;
    const qreal scaledH = h * m_scale;
    const qreal x = (w - scaledW) / 2.0;
    const qreal y = (h - scaledH) / 2.0;

    m_renderer->render(&painter, QRectF(x, y, scaledW, scaledH));
}
