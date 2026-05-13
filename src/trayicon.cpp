#include "trayicon.h"

#include <QApplication>
#include <QPixmap>
#include <QPainter>

TrayIcon::TrayIcon(QObject *parent)
    : QObject(parent)
{
    m_tray = new QSystemTrayIcon(this);
    buildMenu();
    setIcon(true);

    connect(m_tray, &QSystemTrayIcon::activated,
            this,   &TrayIcon::onActivated);
}

void TrayIcon::buildMenu()
{
    m_menu = new QMenu();

    QAction *settingsAction = m_menu->addAction(tr("Settings"));
    connect(settingsAction, &QAction::triggered, this, &TrayIcon::settingsRequested);

    QMenu *pauseMenu = m_menu->addMenu(tr("Pause"));
    const QList<QPair<QString,int>> pauseOptions = {
        { tr("5 minutes"),    5  },
        { tr("10 minutes"),  10  },
        { tr("15 minutes"),  15  },
        { tr("20 minutes"),  20  },
        { tr("30 minutes"),  30  },
        { tr("Indefinitely"), 0  },
    };
    for (const auto &opt : pauseOptions) {
        QAction *a = pauseMenu->addAction(opt.first);
        int minutes = opt.second;
        connect(a, &QAction::triggered, this, [this, minutes]() {
            emit pauseRequested(minutes);
        });
    }
    pauseMenu->addSeparator();
    m_resumeAction = pauseMenu->addAction(tr("Resume"));
    connect(m_resumeAction, &QAction::triggered, this, &TrayIcon::resumeRequested);
    m_resumeAction->setVisible(false);

    QAction *longBreakAction = m_menu->addAction(tr("Take Long Break Now"));
    connect(longBreakAction, &QAction::triggered, this, &TrayIcon::longBreakRequested);

    m_menu->addSeparator();

    QAction *quitAction = m_menu->addAction(tr("Quit"));
    connect(quitAction, &QAction::triggered, this, &TrayIcon::quitRequested);

    m_tray->setContextMenu(m_menu);
}

void TrayIcon::show()
{
    m_tray->show();
}

void TrayIcon::hide()
{
    m_tray->hide();
}

void TrayIcon::setIcon(bool active)
{
    QString iconPath = active ? ":/icons/app_icon.svg" : ":/icons/app_icon_gray.svg";
    QIcon icon(iconPath);
    if (icon.isNull()) {
        // Fallback: draw a simple coloured circle
        QPixmap px(32, 32);
        px.fill(Qt::transparent);
        QPainter p(&px);
        p.setRenderHint(QPainter::Antialiasing);
        p.setBrush(active ? QColor(30, 120, 220) : QColor(120, 120, 120));
        p.setPen(Qt::NoPen);
        p.drawEllipse(px.rect().adjusted(2, 2, -2, -2));
        icon = QIcon(px);
    }
    m_tray->setIcon(icon);
    m_tray->setToolTip(active ? tr("CrossEyeLeopApp") : tr("CrossEyeLeopApp (paused)"));
}

void TrayIcon::setPaused(bool paused)
{
    m_paused = paused;
    setIcon(!paused);
    m_resumeAction->setVisible(paused);
}

void TrayIcon::showNotification(const QString &title, const QString &message, int durationMs)
{
    m_tray->showMessage(title, message, QSystemTrayIcon::Information, durationMs);
}

void TrayIcon::onActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::DoubleClick) {
        emit settingsRequested();
    }
}
