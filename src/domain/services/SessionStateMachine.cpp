#include "SessionStateMachine.h"

namespace cross_eyeleop::domain {

void SessionStateMachine::start() {
    mode_ = SessionMode::Focus;
}

void SessionStateMachine::stop() {
    mode_ = SessionMode::Idle;
}

void SessionStateMachine::toFocus() {
    mode_ = SessionMode::Focus;
}

void SessionStateMachine::toBreak() {
    mode_ = SessionMode::Break;
}

SessionMode SessionStateMachine::mode() const {
    return mode_;
}

} // namespace cross_eyeleop::domain
