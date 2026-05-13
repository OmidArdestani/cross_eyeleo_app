#include "statemachine.h"

StateMachine::StateMachine(QObject *parent)
    : QObject(parent)
{
}

bool StateMachine::transitionTo(AppState newState)
{
    if (!isValidTransition(m_state, newState)) {
        return false;
    }
    m_state = newState;
    emit stateChanged(m_state);
    return true;
}

bool StateMachine::isInBreak() const
{
    return m_state == AppState::RELAXING || m_state == AppState::AUTO_RELAX;
}

bool StateMachine::isSuspended() const
{
    return m_state == AppState::SUSPENDED;
}

bool StateMachine::isValidTransition(AppState from, AppState to)
{
    // Allow any transition from NONE (startup)
    if (from == AppState::NONE) return true;

    // Allow transition to DESTROY from any state
    if (to == AppState::DESTROY) return true;

    // Allow transition to SUSPENDED from most states
    if (to == AppState::SUSPENDED &&
        from != AppState::DESTROY) return true;

    // Allow resume from SUSPENDED to IDLE
    if (from == AppState::SUSPENDED && to == AppState::IDLE) return true;

    switch (from) {
    case AppState::FIRST_LAUNCH:
        return to == AppState::IDLE;

    case AppState::IDLE:
        return to == AppState::WAITING_SCREEN
            || to == AppState::START_BIG_PAUSE
            || to == AppState::AUTO_RELAX
            || to == AppState::RELAXING
            || to == AppState::SUSPENDED;

    case AppState::WAITING_SCREEN:
        return to == AppState::START_BIG_PAUSE
            || to == AppState::IDLE;

    case AppState::START_BIG_PAUSE:
        return to == AppState::RELAXING
            || to == AppState::IDLE;

    case AppState::AUTO_RELAX:
        return to == AppState::RELAXING
            || to == AppState::IDLE;

    case AppState::RELAXING:
        return to == AppState::IDLE;

    default:
        return false;
    }
}
