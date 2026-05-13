#include "exercisemanager.h"

#include <QRandomGenerator>

ExerciseManager::ExerciseManager(QObject *parent)
    : QObject(parent)
{
}

QList<Exercise> ExerciseManager::allExercises()
{
    return {
        {
            ExerciseType::ROLL,
            QObject::tr("Eye Roll"),
            QObject::tr("Slowly roll your eyes in a full circle, 3 times clockwise then 3 times counter-clockwise."),
            ":/images/exercise_roll.svg",
            20
        },
        {
            ExerciseType::LOOK_VERT,
            QObject::tr("Look Up & Down"),
            QObject::tr("Look as far up as you can, hold for 2 seconds, then look down. Repeat 5 times."),
            ":/images/exercise_look_vert.svg",
            20
        },
        {
            ExerciseType::LOOK_HORZ,
            QObject::tr("Look Left & Right"),
            QObject::tr("Look as far left as you can, hold for 2 seconds, then look right. Repeat 5 times."),
            ":/images/exercise_look_horz.svg",
            20
        },
        {
            ExerciseType::CLOSE_TIGHTLY,
            QObject::tr("Close Eyes Tightly"),
            QObject::tr("Close your eyes as tightly as possible for 5 seconds, then relax. Repeat 3 times."),
            ":/images/exercise_close_tightly.svg",
            20
        },
        {
            ExerciseType::BLINK,
            QObject::tr("Rapid Blinking"),
            QObject::tr("Blink rapidly 20 times to lubricate your eyes. Pause and repeat twice more."),
            ":/images/exercise_blink.svg",
            20
        },
        {
            ExerciseType::WINDOW,
            QObject::tr("Focus Shift (20-20-20)"),
            QObject::tr("Look at something 20 feet (6 meters) away through a window for 20 seconds."),
            ":/images/exercise_window.svg",
            20
        },
        {
            ExerciseType::STRETCH,
            QObject::tr("Neck Stretch"),
            QObject::tr("Gently tilt your head to each side for 10 seconds. Roll your shoulders back twice."),
            ":/images/exercise_stretch.svg",
            20
        }
    };
}

Exercise ExerciseManager::randomExercise()
{
    QList<Exercise> exercises = allExercises();
    int index = static_cast<int>(QRandomGenerator::global()->bounded(exercises.size()));
    return exercises.at(index);
}

Exercise ExerciseManager::exerciseForType(ExerciseType type)
{
    const QList<Exercise> exercises = allExercises();
    for (const Exercise &ex : exercises) {
        if (ex.type == type) return ex;
    }
    return exercises.first();
}
