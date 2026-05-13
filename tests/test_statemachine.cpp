#include <QtTest/QtTest>
#include "statemachine.h"

class TestStateMachine : public QObject
{
    Q_OBJECT

private slots:
    void testInitialState();
    void testValidTransitions();
    void testInvalidTransitions();
    void testSignalFired();
    void testConvenienceMethods();
};

void TestStateMachine::testInitialState()
{
    StateMachine sm;
    QCOMPARE(sm.currentState(), AppState::NONE);
}

void TestStateMachine::testValidTransitions()
{
    StateMachine sm;

    // From NONE to FIRST_LAUNCH
    QVERIFY(sm.transitionTo(AppState::FIRST_LAUNCH));
    QCOMPARE(sm.currentState(), AppState::FIRST_LAUNCH);

    // From FIRST_LAUNCH to IDLE
    QVERIFY(sm.transitionTo(AppState::IDLE));
    QCOMPARE(sm.currentState(), AppState::IDLE);

    // From IDLE to WAITING_SCREEN
    QVERIFY(sm.transitionTo(AppState::WAITING_SCREEN));
    QCOMPARE(sm.currentState(), AppState::WAITING_SCREEN);

    // From WAITING_SCREEN to START_BIG_PAUSE
    QVERIFY(sm.transitionTo(AppState::START_BIG_PAUSE));
    QCOMPARE(sm.currentState(), AppState::START_BIG_PAUSE);

    // From START_BIG_PAUSE to RELAXING
    QVERIFY(sm.transitionTo(AppState::RELAXING));
    QCOMPARE(sm.currentState(), AppState::RELAXING);

    // From RELAXING to IDLE
    QVERIFY(sm.transitionTo(AppState::IDLE));
    QCOMPARE(sm.currentState(), AppState::IDLE);

    // IDLE to SUSPENDED
    QVERIFY(sm.transitionTo(AppState::SUSPENDED));
    QCOMPARE(sm.currentState(), AppState::SUSPENDED);

    // SUSPENDED to IDLE (resume)
    QVERIFY(sm.transitionTo(AppState::IDLE));
    QCOMPARE(sm.currentState(), AppState::IDLE);

    // Transition to DESTROY from any state
    QVERIFY(sm.transitionTo(AppState::DESTROY));
    QCOMPARE(sm.currentState(), AppState::DESTROY);
}

void TestStateMachine::testInvalidTransitions()
{
    StateMachine sm;
    sm.transitionTo(AppState::IDLE);

    // IDLE -> RELAXING directly via AUTO_RELAX is valid, but RELAXING -> WAITING_SCREEN is not
    sm.transitionTo(AppState::RELAXING);
    QVERIFY(!sm.transitionTo(AppState::WAITING_SCREEN));
    QCOMPARE(sm.currentState(), AppState::RELAXING); // unchanged

    // FIRST_LAUNCH -> RELAXING is invalid
    StateMachine sm2;
    sm2.transitionTo(AppState::FIRST_LAUNCH);
    QVERIFY(!sm2.transitionTo(AppState::RELAXING));
    QCOMPARE(sm2.currentState(), AppState::FIRST_LAUNCH);
}

void TestStateMachine::testSignalFired()
{
    qRegisterMetaType<AppState>("AppState");
    StateMachine sm;
    QSignalSpy spy(&sm, &StateMachine::stateChanged);

    sm.transitionTo(AppState::IDLE);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).value<AppState>(), AppState::IDLE);

    sm.transitionTo(AppState::SUSPENDED);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.at(1).at(0).value<AppState>(), AppState::SUSPENDED);

    // Invalid transition should NOT emit
    sm.transitionTo(AppState::RELAXING); // SUSPENDED -> RELAXING is invalid
    QCOMPARE(spy.count(), 2);
}

void TestStateMachine::testConvenienceMethods()
{
    StateMachine sm;
    QVERIFY(!sm.isInBreak());
    QVERIFY(!sm.isSuspended());

    sm.transitionTo(AppState::IDLE);
    sm.transitionTo(AppState::RELAXING);
    QVERIFY(sm.isInBreak());
    QVERIFY(!sm.isSuspended());

    sm.transitionTo(AppState::IDLE);
    sm.transitionTo(AppState::AUTO_RELAX);
    QVERIFY(sm.isInBreak());

    sm.transitionTo(AppState::IDLE);
    sm.transitionTo(AppState::SUSPENDED);
    QVERIFY(!sm.isInBreak());
    QVERIFY(sm.isSuspended());
}

QTEST_MAIN(TestStateMachine)
#include "test_statemachine.moc"
