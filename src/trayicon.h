#pragma once

#include <QObject>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>

class TrayIcon : public QObject
{
    Q_OBJECT

public:
    explicit TrayIcon(QObject *parent = nullptr);

    void show();
    void hide();

    void setIcon(bool active);
    void setPaused(bool paused);

    void showNotification(const QString &title, const QString &message, int durationMs = 5000);

signals:
    void settingsRequested();
    void pauseRequested(int minutes); // 0 = indefinitely
    void resumeRequested();
    void longBreakRequested();
    void quitRequested();

private slots:
    void onActivated(QSystemTrayIcon::ActivationReason reason);

private:
    void buildMenu();

    QSystemTrayIcon *m_tray{nullptr};
    QMenu           *m_menu{nullptr};
    QAction         *m_resumeAction{nullptr};
    bool             m_paused{false};
};
