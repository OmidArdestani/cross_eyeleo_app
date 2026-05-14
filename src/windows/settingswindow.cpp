#include "settingswindow.h"
#include "settingsmanager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QDesktopServices>
#include <QUrl>

SettingsWindow::SettingsWindow(SettingsManager *settings, QWidget *parent)
    : QDialog(parent)
    , m_settings(settings)
{
    setWindowTitle(tr("Settings - CrossEyeLeoApp"));
    setMinimumSize(480, 420);
    setupUi();
}

void SettingsWindow::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    m_tabs = new QTabWidget(this);

    QWidget *longBreakTab  = new QWidget();
    QWidget *shortBreakTab = new QWidget();
    QWidget *otherTab      = new QWidget();
    QWidget *infoTab       = new QWidget();

    setupLongBreakTab(longBreakTab);
    setupShortBreakTab(shortBreakTab);
    setupOtherTab(otherTab);
    setupInfoTab(infoTab);

    m_tabs->addTab(longBreakTab,  tr("Long Break"));
    m_tabs->addTab(shortBreakTab, tr("Short Break"));
    m_tabs->addTab(otherTab,      tr("Other Settings"));
    m_tabs->addTab(infoTab,       tr("Information"));

    mainLayout->addWidget(m_tabs);

    // Bottom buttons
    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *tryShort  = new QPushButton(tr("Try Short Break"), this);
    QPushButton *tryLong   = new QPushButton(tr("Try Long Break"), this);
    QPushButton *okBtn     = new QPushButton(tr("OK"), this);
    QPushButton *cancelBtn = new QPushButton(tr("Cancel"), this);
    QPushButton *applyBtn  = new QPushButton(tr("Apply"), this);

    connect(tryShort,  &QPushButton::clicked, this, &SettingsWindow::tryShortBreak);
    connect(tryLong,   &QPushButton::clicked, this, &SettingsWindow::tryLongBreak);
    connect(okBtn,     &QPushButton::clicked, this, &SettingsWindow::onOk);
    connect(cancelBtn, &QPushButton::clicked, this, &SettingsWindow::onCancel);
    connect(applyBtn,  &QPushButton::clicked, this, &SettingsWindow::onApply);

    btnLayout->addWidget(tryShort);
    btnLayout->addWidget(tryLong);
    btnLayout->addStretch();
    btnLayout->addWidget(okBtn);
    btnLayout->addWidget(cancelBtn);
    btnLayout->addWidget(applyBtn);

    mainLayout->addLayout(btnLayout);
}

void SettingsWindow::setupLongBreakTab(QWidget *tab)
{
    QVBoxLayout *vl = new QVBoxLayout(tab);
    vl->setSpacing(16);
    vl->setContentsMargins(16, 16, 16, 16);

    m_bigPauseEnabled = new QCheckBox(tr("Enable long break"), tab);
    vl->addWidget(m_bigPauseEnabled);

    QFormLayout *fl = new QFormLayout();
    m_bigPauseInterval = new QSpinBox(tab);
    m_bigPauseInterval->setRange(10, 90);
    m_bigPauseInterval->setSuffix(tr(" minutes"));
    fl->addRow(tr("Interval:"), m_bigPauseInterval);

    m_bigPauseDuration = new QSpinBox(tab);
    m_bigPauseDuration->setRange(1, 15);
    m_bigPauseDuration->setSuffix(tr(" minutes"));
    fl->addRow(tr("Duration:"), m_bigPauseDuration);

    vl->addLayout(fl);
    vl->addStretch();
}

void SettingsWindow::setupShortBreakTab(QWidget *tab)
{
    QVBoxLayout *vl = new QVBoxLayout(tab);
    vl->setSpacing(16);
    vl->setContentsMargins(16, 16, 16, 16);

    m_miniPauseEnabled = new QCheckBox(tr("Enable short break"), tab);
    vl->addWidget(m_miniPauseEnabled);

    QFormLayout *fl = new QFormLayout();
    m_miniPauseInterval = new QSpinBox(tab);
    m_miniPauseInterval->setRange(1, 60);
    m_miniPauseInterval->setSuffix(tr(" minutes"));
    fl->addRow(tr("Interval:"), m_miniPauseInterval);

    m_miniPauseDuration = new QSpinBox(tab);
    m_miniPauseDuration->setRange(5, 60);
    m_miniPauseDuration->setSuffix(tr(" seconds"));
    fl->addRow(tr("Duration:"), m_miniPauseDuration);

    vl->addLayout(fl);

    m_miniPauseFullscreen = new QCheckBox(tr("Show fullscreen"), tab);
    vl->addWidget(m_miniPauseFullscreen);
    vl->addStretch();
}

void SettingsWindow::setupOtherTab(QWidget *tab)
{
    QVBoxLayout *vl = new QVBoxLayout(tab);
    vl->setSpacing(12);
    vl->setContentsMargins(16, 16, 16, 16);

    QGroupBox *warnGroup = new QGroupBox(tr("Warnings"), tab);
    QHBoxLayout *whl = new QHBoxLayout(warnGroup);
    m_warningEnabled = new QCheckBox(tr("Warn before long break"), warnGroup);
    m_warningInterval = new QSpinBox(warnGroup);
    m_warningInterval->setRange(1, 30);
    m_warningInterval->setSuffix(tr(" min before"));
    whl->addWidget(m_warningEnabled);
    whl->addWidget(m_warningInterval);
    vl->addWidget(warnGroup);

    QGroupBox *behavGroup = new QGroupBox(tr("Behavior"), tab);
    QVBoxLayout *bvl = new QVBoxLayout(behavGroup);
    m_soundsEnabled      = new QCheckBox(tr("Enable sounds"), behavGroup);
    m_strictMode         = new QCheckBox(tr("Strict mode (cannot skip)"), behavGroup);
    m_windowNearby       = new QCheckBox(tr("Only show when near PC"), behavGroup);
    m_inactivityTracking = new QCheckBox(tr("Track inactivity"), behavGroup);
    bvl->addWidget(m_soundsEnabled);
    bvl->addWidget(m_strictMode);
    bvl->addWidget(m_windowNearby);
    bvl->addWidget(m_inactivityTracking);
    vl->addWidget(behavGroup);

    QGroupBox *notifGroup = new QGroupBox(tr("Notifications"), tab);
    QVBoxLayout *nvl = new QVBoxLayout(notifGroup);
    m_showNotifications     = new QCheckBox(tr("Show notifications"), notifGroup);
    m_canCloseNotifications = new QCheckBox(tr("Allow closing notifications"), notifGroup);
    nvl->addWidget(m_showNotifications);
    nvl->addWidget(m_canCloseNotifications);
    vl->addWidget(notifGroup);

    vl->addStretch();
}

void SettingsWindow::setupInfoTab(QWidget *tab)
{
    QVBoxLayout *vl = new QVBoxLayout(tab);
    vl->setSpacing(12);
    vl->setContentsMargins(16, 16, 16, 16);

    QLabel *appLabel = new QLabel(
        "<b>CrossEyeLeoApp</b> v1.0.0<br/>"
        "A cross-platform eye exercise reminder.", tab);
    appLabel->setAlignment(Qt::AlignCenter);
    vl->addWidget(appLabel);

    QGroupBox *statsGroup = new QGroupBox(tr("Statistics"), tab);
    QFormLayout *fl = new QFormLayout(statsGroup);

    m_shortBreakCountLabel  = new QLabel("0", statsGroup);
    m_longBreakCountLabel   = new QLabel("0", statsGroup);
    m_skipCountLabel        = new QLabel("0", statsGroup);
    m_postponeCountLabel    = new QLabel("0", statsGroup);
    m_refuseCountLabel      = new QLabel("0", statsGroup);

    fl->addRow(tr("Short breaks completed:"), m_shortBreakCountLabel);
    fl->addRow(tr("Long breaks completed:"),  m_longBreakCountLabel);
    fl->addRow(tr("Breaks skipped:"),         m_skipCountLabel);
    fl->addRow(tr("Breaks postponed:"),       m_postponeCountLabel);
    fl->addRow(tr("Breaks refused:"),         m_refuseCountLabel);

    vl->addWidget(statsGroup);

    QGroupBox *aboutGroup = new QGroupBox(tr("About the Developer"), tab);
    QFormLayout *afl = new QFormLayout(aboutGroup);

    QLabel *emailLabel = new QLabel(
        "<a href=\"mailto:omid.91wo@gmail.com\">omid.91wo@gmail.com</a>", aboutGroup);
    emailLabel->setOpenExternalLinks(true);

    QLabel *linkedInLabel = new QLabel(
        "<a href=\"https://www.linkedin.com/in/omidardestani/\">linkedin.com/in/omidardestani</a>",
        aboutGroup);
    linkedInLabel->setOpenExternalLinks(true);

    QLabel *githubLabel = new QLabel(
        "<a href=\"https://github.com/OmidArdestani\">github.com/OmidArdestani</a>", aboutGroup);
    githubLabel->setOpenExternalLinks(true);

    afl->addRow(tr("Email:"),    emailLabel);
    afl->addRow(tr("LinkedIn:"), linkedInLabel);
    afl->addRow(tr("GitHub:"),   githubLabel);

    vl->addWidget(aboutGroup);

    vl->addStretch();
}

void SettingsWindow::loadFromSettings()
{
    m_bigPauseEnabled->setChecked(m_settings->bigPauseEnabled());
    m_bigPauseInterval->setValue(m_settings->bigPauseInterval());
    m_bigPauseDuration->setValue(m_settings->bigPauseDuration());

    m_miniPauseEnabled->setChecked(m_settings->miniPauseEnabled());
    m_miniPauseInterval->setValue(m_settings->miniPauseInterval());
    m_miniPauseDuration->setValue(m_settings->miniPauseDuration());
    m_miniPauseFullscreen->setChecked(m_settings->miniPauseFullscreen());

    m_warningEnabled->setChecked(m_settings->warningEnabled());
    m_warningInterval->setValue(m_settings->warningInterval());

    m_soundsEnabled->setChecked(m_settings->soundsEnabled());
    m_strictMode->setChecked(m_settings->strictMode());
    m_windowNearby->setChecked(m_settings->windowNearby());
    m_inactivityTracking->setChecked(m_settings->inactivityTracking());
    m_canCloseNotifications->setChecked(m_settings->canCloseNotifications());
    m_showNotifications->setChecked(m_settings->showNotifications());

    m_shortBreakCountLabel->setText(QString::number(m_settings->shortBreakCount()));
    m_longBreakCountLabel->setText(QString::number(m_settings->longBreakCount()));
    m_skipCountLabel->setText(QString::number(m_settings->skipCount()));
    m_postponeCountLabel->setText(QString::number(m_settings->postponeCount()));
    m_refuseCountLabel->setText(QString::number(m_settings->refuseCount()));
}

void SettingsWindow::saveToSettings()
{
    m_settings->setBigPauseEnabled(m_bigPauseEnabled->isChecked());
    m_settings->setBigPauseInterval(m_bigPauseInterval->value());
    m_settings->setBigPauseDuration(m_bigPauseDuration->value());

    m_settings->setMiniPauseEnabled(m_miniPauseEnabled->isChecked());
    m_settings->setMiniPauseInterval(m_miniPauseInterval->value());
    m_settings->setMiniPauseDuration(m_miniPauseDuration->value());
    m_settings->setMiniPauseFullscreen(m_miniPauseFullscreen->isChecked());

    m_settings->setWarningEnabled(m_warningEnabled->isChecked());
    m_settings->setWarningInterval(m_warningInterval->value());

    m_settings->setSoundsEnabled(m_soundsEnabled->isChecked());
    m_settings->setStrictMode(m_strictMode->isChecked());
    m_settings->setWindowNearby(m_windowNearby->isChecked());
    m_settings->setInactivityTracking(m_inactivityTracking->isChecked());
    m_settings->setCanCloseNotifications(m_canCloseNotifications->isChecked());
    m_settings->setShowNotifications(m_showNotifications->isChecked());

    m_settings->save();
}

void SettingsWindow::onOk()
{
    saveToSettings();
    accept();
}

void SettingsWindow::onApply()
{
    saveToSettings();
}

void SettingsWindow::onCancel()
{
    reject();
}
