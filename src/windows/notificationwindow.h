#pragma once

#include <QWidget>
#include <QLabel>
#include <QPropertyAnimation>
#include <QTimer>

class SettingsManager;

class NotificationWindow : public QWidget
{
    Q_OBJECT

public:
    explicit NotificationWindow(SettingsManager *settings = nullptr, QWidget *parent = nullptr);

    void showNotification(const QString &message);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void onAutoHide();
    void onHideFinished() ;

private:
    void setupUi();
    void positionWindow();

    SettingsManager    *m_settings{nullptr};
    QLabel             *m_messageLabel{nullptr};
    QPropertyAnimation *m_showAnim{nullptr};
    QPropertyAnimation *m_hideAnim{nullptr};
    QTimer             *m_autoHideTimer{nullptr};
};
