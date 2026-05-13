#pragma once

namespace cross_eyeleop::domain {

enum class SessionMode {
    Idle,
    Focus,
    Break
};

class SessionStateMachine {
public:
    void start();
    void stop();
    void toFocus();
    void toBreak();

    [[nodiscard]] SessionMode mode() const;

private:
    SessionMode mode_{SessionMode::Idle};
};

} // namespace cross_eyeleop::domain
