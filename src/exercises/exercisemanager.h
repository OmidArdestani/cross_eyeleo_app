#pragma once

#include <QObject>
#include <QList>
#include <QString>

enum class ExerciseType {
    ROLL,
    LOOK_VERT,
    LOOK_HORZ,
    CLOSE_TIGHTLY,
    BLINK,
    WINDOW,
    STRETCH
};

struct Exercise {
    ExerciseType type;
    QString name;
    QString instruction;
    QString imagePath;
    int defaultDurationSeconds;
};

class ExerciseManager : public QObject
{
    Q_OBJECT

public:
    explicit ExerciseManager(QObject *parent = nullptr);

    static QList<Exercise> allExercises();
    static Exercise randomExercise();
    static Exercise exerciseForType(ExerciseType type);
};
