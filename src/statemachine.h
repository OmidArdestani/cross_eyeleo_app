#pragma once

#include <QDataStream>
#include <QDebug>
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

// Explicit stream/debug operators are required so that Q_DECLARE_METATYPE
// and MOC-generated QMetaTypeInterfaceWrapper<AppState> work with GCC.
// Without them GCC cannot resolve which overload to pick for an enum class
// whose underlying type is 'int', producing "ambiguous overload" errors.

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

inline QDebug operator<<(QDebug dbg, AppState v)
{
    dbg.nospace() << "AppState(" << static_cast<int>(v) << ')';
    return dbg;
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
