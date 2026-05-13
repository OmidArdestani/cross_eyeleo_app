#pragma once

#include <QObject>
#include <QTimer>

namespace cross_eyeleop::infrastructure {

class QtHeartbeat final : public QObject {
    Q_OBJECT

public:
    explicit QtHeartbeat(QObject* parent = nullptr);

    void start();
    void stop();

signals:
    void pulse();

private:
    QTimer timer_;
};

} // namespace cross_eyeleop::infrastructure
