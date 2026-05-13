#include "MainWindow.h"

#include <QVBoxLayout>
#include <QWidget>

namespace cross_eyeleop::presentation {

MainWindow::MainWindow(MainViewModel& viewModel, QWidget* parent)
    : QMainWindow(parent)
    , viewModel_(viewModel) {
    setWindowTitle("Cross Eyeleop");
    resize(420, 240);

    auto* central = new QWidget(this);
    auto* layout = new QVBoxLayout(central);

    statusLabel_ = new QLabel(this);
    timerLabel_ = new QLabel(this);
    actionButton_ = new QPushButton(this);

    auto timerFont = timerLabel_->font();
    timerFont.setPointSize(24);
    timerLabel_->setFont(timerFont);

    layout->addWidget(statusLabel_);
    layout->addWidget(timerLabel_);
    layout->addWidget(actionButton_);
    layout->addStretch(1);

    setCentralWidget(central);

    statusLabel_->setText(viewModel_.statusText());
    timerLabel_->setText(viewModel_.countdownText());
    actionButton_->setText(viewModel_.buttonText());

    connect(actionButton_, &QPushButton::clicked, &viewModel_, &MainViewModel::toggleSession);
    connect(&viewModel_, &MainViewModel::statusChanged, statusLabel_, &QLabel::setText);
    connect(&viewModel_, &MainViewModel::countdownChanged, timerLabel_, &QLabel::setText);
    connect(&viewModel_, &MainViewModel::buttonTextChanged, actionButton_, &QPushButton::setText);
}

} // namespace cross_eyeleop::presentation
