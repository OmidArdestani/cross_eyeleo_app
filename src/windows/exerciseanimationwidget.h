#pragma once

#include <QWidget>
#include <QSvgRenderer>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>

/**
 * @brief A widget that renders an SVG exercise illustration with a
 *        continuous breathing/pulsing animation to convey that the
 *        exercise is active rather than just showing a still image.
 */
class ExerciseAnimationWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal animScale READ animScale WRITE setAnimScale)

public:
    explicit ExerciseAnimationWidget(QWidget *parent = nullptr);

    void setImage(const QString &svgPath);
    void startAnimation();
    void stopAnimation();

    QSize sizeHint() const override { return QSize(120, 120); }

    qreal animScale() const { return m_scale; }
    void  setAnimScale(qreal scale);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QSvgRenderer              *m_renderer{nullptr};
    QSequentialAnimationGroup *m_animGroup{nullptr};
    qreal                      m_scale{1.0};
};
