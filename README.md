# Cross-platform EyeLeo App (Inspired by [EyeLeo](https://blucbo.github.io/dist/index.html))

A cross-platform eye-care reminder application built with Qt 6 and C++17. The app runs silently in the system tray and periodically prompts users to take structured short and long breaks, including guided eye exercises. Its goal is to reduce digital eye strain and support healthier screen habits during extended computer use.


---

## Features

- **System tray integration** — unobtrusive icon with a right-click menu for quick access to settings, manual breaks, and pause/resume.
- **Long breaks** (big pause) — full-screen overlay with guided eye exercises; configurable interval and duration.
- **Short breaks** (mini pause) — lightweight overlay reminding you to look away; configurable interval and duration.
- **Pre-break warning** — optional countdown window shown before a long break begins, with "Ready" and "Postpone" buttons.
- **Toast notifications** — brief on-screen reminders about the next upcoming long break.
- **7 guided eye exercises**:
  - Roll, Look Vertically, Look Horizontally, Close Tightly, Blink, Look out the Window, Stretch
- **Inactivity detection** — timers automatically pause while you're away from the keyboard and resume when you return (Linux: via `libxss`; Windows: `GetLastInputInfo`; macOS: CoreGraphics).
- **Statistics** — tracks completed long/short breaks, skips, postpones, and refusals.
- **Strict mode** — prevents dismissing break windows early.
- **Settings window** — tabbed UI covering all options and live statistics.
- **Single instance** — prevents multiple copies of the app from running simultaneously.
- **Persistent settings** — all preferences saved automatically via `QSettings`.

---

## Screenshots

> _Add screenshots here once the application is running._

---

## Requirements

| Dependency | Version |
|---|---|
| Qt | ≥ 6.6.0 (Core, Gui, Widgets, Multimedia, Svg) |
| CMake | ≥ 3.19 |
| C++ compiler | C++17 (GCC 10+, Clang 12+, MSVC 2019+) |
| **Linux only** | `libxss-dev` (optional — enables idle detection) |

---

## Building

### Quick start

```bash
# Clone the repository
git clone https://github.com/OmidArdestani/cross_eyeleo_app.git
cd cross_eyeleo_app

# Configure and build
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

### Linux — additional dependency

```bash
sudo apt-get install -y libxss-dev   # Ubuntu/Debian
```

### Build options

| CMake option | Default | Description |
|---|---|---|
| `CROSSEYELEO_BUILD_APP` | `ON` | Build the main application |
| `CROSSEYELEO_BUILD_TESTS` | `OFF` | Build the unit test suite |

### Run

```bash
./build/CrossEyeLeoApp          # Linux
open build/CrossEyeLeoApp.app   # macOS
build\Release\CrossEyeLeoApp.exe  # Windows
```

---

## Running Tests

Tests are built separately to avoid pulling in GUI dependencies:

```bash
cmake -B build-tests -DCROSSEYELEO_BUILD_APP=OFF -DCROSSEYELEO_BUILD_TESTS=ON
cmake --build build-tests --parallel

# Linux/macOS (headless)
cd build-tests && QT_QPA_PLATFORM=offscreen ctest --output-on-failure

# Windows
cd build-tests && ctest -C Release --output-on-failure
```

---

## Project Structure

```
cross_eyeleo_app/
├── src/
│   ├── main.cpp                   Entry point; single QApplication instance
│   ├── app.cpp / app.h            Top-level orchestrator
│   ├── settingsmanager.cpp/.h     Persistent settings (QSettings wrapper)
│   ├── statemachine.cpp/.h        Application state machine (AppState enum)
│   ├── taskscheduler.cpp/.h       Break and warning timers
│   ├── activitymonitor.cpp/.h     Platform idle-time detection
│   ├── trayicon.cpp/.h            System tray icon and context menu
│   ├── windows/
│   │   ├── settingswindow         Full settings + statistics dialog
│   │   ├── bigpausewindow         Full-screen long break overlay
│   │   ├── minipausewindow        Short break overlay
│   │   ├── beforepausewindow      Pre-break countdown dialog
│   │   └── notificationwindow     Toast notification
│   └── exercises/
│       └── exercisemanager        Exercise catalogue (7 types)
├── resources/
│   ├── icons/                     SVG app icons (active + gray)
│   └── images/                    SVG exercise illustrations
├── tests/
│   ├── test_settingsmanager.cpp   Unit tests for SettingsManager
│   └── test_statemachine.cpp      Unit tests for StateMachine
└── .github/workflows/
    ├── build.yml                  CI: build + test on Linux, Windows, macOS
    └── release.yml                Release: DEB/TGZ, ZIP, DMG on tagged commits
```

---

## CI / CD

GitHub Actions automatically:

- **On every push / pull request to `main`**: builds on Ubuntu, Windows, and macOS and runs the unit test suite.
- **On a `v*` tag**: builds release packages and uploads them to the GitHub Release:
  - Linux: `.deb` and `.tar.gz`
  - Windows: `.zip`
  - macOS: `.dmg`

---

## Configuration Defaults

| Setting | Default |
|---|---|
| Long break interval | 60 minutes |
| Long break duration | 5 minutes |
| Short break interval | 10 minutes |
| Short break duration | 20 seconds |
| Warning before long break | 1 minute |
| Inactivity tracking | Enabled |
| Sounds | Enabled |
| Strict mode | Disabled |

All settings are adjustable in the **Settings** window (right-click tray icon → Settings).

---

## License

MIT © 2026 Omid Ardestani — see [LICENSE](LICENSE) for details.
