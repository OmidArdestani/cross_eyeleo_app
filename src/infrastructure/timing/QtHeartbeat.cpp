#include "QtHeartbeat.h"

namespace cross_eyeleop::infrastructure {

QtHeartbeat::QtHeartbeat(QObject* parent)
    : QObject(parent) {
    timer_.setInterval(1000);
    connect(&timer_, &QTimer::timeout, this, &QtHeartbeat::pulse);
}

void QtHeartbeat::start() {
    timer_.start();
}

void QtHeartbeat::stop() {
    timer_.stop();
}

} // namespace cross_eyeleop::infrastructure
