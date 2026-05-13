#include <QtTest/QtTest>
#include "settingsmanager.h"

class TestSettingsManager : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testDefaultValues();
    void testSaveAndLoad();
    void testSignalEmission();

private:
    SettingsManager *m_settings{nullptr};
};

void TestSettingsManager::initTestCase()
{
    // Use unique org/app to avoid polluting real settings
    QCoreApplication::setOrganizationName("CrossEyeLeop_Test");
    QCoreApplication::setApplicationName("CrossEyeLeopApp_Test");
    m_settings = new SettingsManager();
}

void TestSettingsManager::cleanupTestCase()
{
    delete m_settings;
    m_settings = nullptr;
}

void TestSettingsManager::testDefaultValues()
{
    SettingsManager sm;
    QCOMPARE(sm.bigPauseEnabled(),       true);
    QCOMPARE(sm.bigPauseInterval(),      60);
    QCOMPARE(sm.bigPauseDuration(),      5);

    QCOMPARE(sm.miniPauseEnabled(),      true);
    QCOMPARE(sm.miniPauseInterval(),     10);
    QCOMPARE(sm.miniPauseDuration(),     20);
    QCOMPARE(sm.miniPauseFullscreen(),   false);

    QCOMPARE(sm.warningEnabled(),        true);
    QCOMPARE(sm.warningInterval(),       1);

    QCOMPARE(sm.soundsEnabled(),         true);
    QCOMPARE(sm.strictMode(),            false);
    QCOMPARE(sm.windowNearby(),          true);
    QCOMPARE(sm.inactivityTracking(),    true);
    QCOMPARE(sm.canCloseNotifications(), true);
    QCOMPARE(sm.showNotifications(),     true);
    QCOMPARE(sm.firstLaunch(),           true);

    QCOMPARE(sm.shortBreakCount(),       0);
    QCOMPARE(sm.longBreakCount(),        0);
    QCOMPARE(sm.skipCount(),             0);
    QCOMPARE(sm.postponeCount(),         0);
    QCOMPARE(sm.refuseCount(),           0);
}

void TestSettingsManager::testSaveAndLoad()
{
    SettingsManager sm;
    sm.setBigPauseEnabled(false);
    sm.setBigPauseInterval(45);
    sm.setBigPauseDuration(3);
    sm.setMiniPauseEnabled(false);
    sm.setMiniPauseInterval(7);
    sm.setMiniPauseDuration(30);
    sm.setStrictMode(true);
    sm.setShortBreakCount(5);
    sm.setLongBreakCount(2);
    sm.save();

    SettingsManager sm2;
    sm2.load();
    QCOMPARE(sm2.bigPauseEnabled(),   false);
    QCOMPARE(sm2.bigPauseInterval(),  45);
    QCOMPARE(sm2.bigPauseDuration(),  3);
    QCOMPARE(sm2.miniPauseEnabled(),  false);
    QCOMPARE(sm2.miniPauseInterval(), 7);
    QCOMPARE(sm2.miniPauseDuration(), 30);
    QCOMPARE(sm2.strictMode(),        true);
    QCOMPARE(sm2.shortBreakCount(),   5);
    QCOMPARE(sm2.longBreakCount(),    2);
}

void TestSettingsManager::testSignalEmission()
{
    SettingsManager sm;
    QSignalSpy spy(&sm, &SettingsManager::settingsChanged);

    sm.setBigPauseEnabled(false);
    QCOMPARE(spy.count(), 1);

    sm.setBigPauseInterval(30);
    QCOMPARE(spy.count(), 2);

    sm.setStrictMode(true);
    QCOMPARE(spy.count(), 3);
}

QTEST_MAIN(TestSettingsManager)
#include "test_settingsmanager.moc"
