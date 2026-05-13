#pragma once

#include <QDataStream>
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

// Explicit QDataStream operators are required so that Q_DECLARE_METATYPE
// works with GCC for an enum class whose underlying type is 'int'.
// Without them, GCC cannot resolve whether to use the char or qint32 overload,
// producing an "ambiguous overload" error during template instantiation.
inline QDataStream &operator<<(QDataStream &ds, AppState v)
{
    return ds << static_cast<qint32>(v);
}

inline QDataStream &operator>>(QDataStream &ds, AppState &v)
{
    qint32 i;
    ds >> i;
    if (i >= static_cast<qint32>(AppState::NONE) &&
        i <= static_cast<qint32>(AppState::DESTROY)) {
        v = static_cast<AppState>(i);
    } else {
        ds.setStatus(QDataStream::ReadCorruptData);
        v = AppState::NONE;
    }
    return ds;
}

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
