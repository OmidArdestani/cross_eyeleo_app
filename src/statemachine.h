#pragma once

#include <QObject>

enum class AppState {
    NONE,
    FIRST_LAUNCH,
    SUSPENDED,
    IDLE,
    WAITING_SCREEN,
    START_BIG_PAUSE,
    AUTO_RELAX,
    RELAXING,
    DESTROY
};

Q_DECLARE_METATYPE(AppState)

class StateMachine : public QObject
{
    Q_OBJECT

public:
    explicit StateMachine(QObject *parent = nullptr);

    AppState currentState() const { return m_state; }

    bool transitionTo(AppState newState);

    bool isInBreak() const;
    bool isSuspended() const;

signals:
    void stateChanged(AppState newState);

private:
    static bool isValidTransition(AppState from, AppState to);

    AppState m_state{AppState::NONE};
};
