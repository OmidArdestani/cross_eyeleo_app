#pragma once

#include <QLabel>
#include <QMainWindow>
#include <QPushButton>

#include "presentation/viewmodels/MainViewModel.h"

namespace cross_eyeleop::presentation {

class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(MainViewModel& viewModel, QWidget* parent = nullptr);

private:
    MainViewModel& viewModel_;
    QLabel* statusLabel_{nullptr};
    QLabel* timerLabel_{nullptr};
    QPushButton* actionButton_{nullptr};
};

} // namespace cross_eyeleop::presentation
