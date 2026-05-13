#pragma once

#include <QDialog>
#include <QTabWidget>
#include <QCheckBox>
#include <QVariant>   // must precede QSpinBox to satisfy qabstractspinbox.h in MOC compilation
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>

class SettingsManager;

class SettingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsWindow(SettingsManager *settings, QWidget *parent = nullptr);

    void loadFromSettings();

signals:
    void tryShortBreak();
    void tryLongBreak();

private slots:
    void onOk();
    void onApply();
    void onCancel();

private:
    void setupUi();
    void setupLongBreakTab(QWidget *tab);
    void setupShortBreakTab(QWidget *tab);
    void setupOtherTab(QWidget *tab);
    void setupInfoTab(QWidget *tab);
    void saveToSettings();

    SettingsManager *m_settings{nullptr};
    QTabWidget      *m_tabs{nullptr};

    // Long break
    QCheckBox *m_bigPauseEnabled{nullptr};
    QSpinBox  *m_bigPauseInterval{nullptr};
    QSpinBox  *m_bigPauseDuration{nullptr};

    // Short break
    QCheckBox *m_miniPauseEnabled{nullptr};
    QSpinBox  *m_miniPauseInterval{nullptr};
    QSpinBox  *m_miniPauseDuration{nullptr};
    QCheckBox *m_miniPauseFullscreen{nullptr};

    // Other
    QCheckBox *m_warningEnabled{nullptr};
    QSpinBox  *m_warningInterval{nullptr};
    QCheckBox *m_soundsEnabled{nullptr};
    QCheckBox *m_strictMode{nullptr};
    QCheckBox *m_windowNearby{nullptr};
    QCheckBox *m_inactivityTracking{nullptr};
    QCheckBox *m_canCloseNotifications{nullptr};
    QCheckBox *m_showNotifications{nullptr};

    // Info / stats labels
    QLabel *m_shortBreakCountLabel{nullptr};
    QLabel *m_longBreakCountLabel{nullptr};
    QLabel *m_skipCountLabel{nullptr};
    QLabel *m_postponeCountLabel{nullptr};
    QLabel *m_refuseCountLabel{nullptr};
};
