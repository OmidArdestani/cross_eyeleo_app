# cross_eyeleop_app

A Qt + C++ desktop app inspired by Eyeleo, built with a layered architecture for maintainability and growth.

## Architecture

The app is intentionally split into deep layers:

- **Domain** (`src/domain`): break rules and session state transitions.
- **Application** (`src/application`): use-case orchestration and snapshot model.
- **Infrastructure** (`src/infrastructure`): Qt adapters (settings + timer heartbeat).
- **Presentation** (`src/presentation`): Qt Widgets view-model and window.

## Build

Requirements:

- CMake 3.24+
- C++20 compiler
- Qt 6 Widgets

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

## Run

```bash
./build/cross_eyeleop_app
```

## Test

```bash
cmake -S . -B build-test -DCROSSEYELEOP_BUILD_APP=OFF -DCROSSEYELEOP_BUILD_TESTS=ON
cmake --build build-test
ctest --test-dir build-test --output-on-failure
```

## Release pipeline

GitHub Actions workflow: `.github/workflows/build-release.yml`

- Builds on **Windows, Linux, macOS**
- Runs tests
- Packages artifacts with CPack
- Publishes release assets automatically for tags matching `v*`
