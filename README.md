# DevPulse

A real-time developer service monitoring dashboard built with **Qt5/C++**.
Monitor your local HTTP APIs, databases, and background workers from a single dashboard.

## Screenshot

![DevPulse Dashboard](screenshot.png)

## Features

### Core
- Add and monitor HTTP and TCP services (localhost:3000, localhost:5432 etc)
- Async health checks — never blocks the UI thread
- Three service states — **Up**, **Degraded** (slow), **Down**
- Configurable check interval and timeout per service
- Response time measurement in milliseconds

### Visualisation
- Live latency graph per service (QChart / QLineSeries)
- Color-coded status — teal for Up, orange for Degraded, red for Down
- Uptime percentage tracker with rolling window calculation
- Per-service timestamped log panel with filter

### Alerts
- Desktop notification on service Down and Recovery
- System tray icon showing overall health
- Tray right-click menu — show/hide/quit

### Profiles
- Save service configs as JSON profiles
- Load profiles to restore full monitoring setup
- Auto-reload last profile on startup

### Utility
- Right-click context menu — Check Now, Copy URL, Remove
- Export logs to text file
- Dark theme UI

## Architecture

```
src/
├── core/                    # Pure backend — zero Qt widgets
│   ├── Service.h            # Domain model (Q_GADGET, enum class)
│   ├── ServiceRepository    # In-memory store with Qt signals
│   ├── MonitorEngine        # Timer-driven check orchestration
│   ├── checkers/
│   │   ├── IChecker.h       # Pure abstract interface
│   │   ├── HttpChecker      # QNetworkAccessManager async HTTP
│   │   ├── TcpChecker       # QTcpSocket async TCP connect
│   │   └── CheckerFactory   # Factory pattern — scheme dispatch
│   ├── monitoring/
│   │   ├── LatencyStore     # Circular buffer — O(1) insert
│   │   └── UptimeTracker    # Rolling window uptime calculation
│   └── storage/
│       └── JsonProfileManager  # QJsonDocument serialisation
├── models/                  # Qt MVC — bridge between core and UI
│   ├── ServiceTableModel    # QAbstractTableModel
│   └── LogModel             # QAbstractListModel
└── ui/                      # All QWidget/QMainWindow code
    ├── MainWindow           # Application shell
    ├── LatencyGraphWidget   # QChart live graph
    ├── LogPanelWidget       # Log display with QSortFilterProxyModel
    ├── TrayManager          # QSystemTrayIcon + notifications
    └── dialogs/
        └── AddServiceDialog # Service configuration dialog
```

## Key Design Decisions

**IChecker Interface** — All checkers implement a pure abstract interface.
The monitoring engine depends only on `IChecker*`, never on concrete implementations.
Adding a new protocol (gRPC, Redis, WebSocket) requires zero changes to existing code.

**QAbstractTableModel** — Service data is fully decoupled from rendering.
The table view asks the model for data — the model never touches the UI.
Multiple views can observe the same model simultaneously.

**Circular Buffer** — Latency history uses a fixed-size ring buffer.
O(1) insert, fixed memory footprint regardless of how long the app runs.
No allocations after startup.

**Repository Pattern** — `ServiceRepository` is the single source of truth.
All mutations go through the repository, which emits signals.
UI components observe signals, never poll.

**Factory Pattern** — `CheckerFactory` centralises protocol dispatch.
URL scheme determines checker type — HTTP/HTTPS → HttpChecker, everything else → TcpChecker.

## Build

### Requirements

| Dependency | Version |
|------------|---------|
| Qt | 5.15+ |
| CMake | 3.16+ |
| GCC / Clang | 9+ / 10+ |
| Ninja | Any |

### Install Dependencies (Ubuntu/Debian)

```bash
sudo apt install \
  qtbase5-dev \
  libqt5charts5-dev \
  libqt5sql5-sqlite \
  cmake \
  ninja-build \
  build-essential
```

### Build and Run

```bash
git clone https://github.com/Negi2110/devpulse.git
cd devpulse
cmake -B build -G Ninja
cmake --build build
./build/devpulse
```

## Usage

### Adding a Service

```
File → Add Service  (Ctrl+N)

Name:               Auth API
URL / Address:      http://localhost:3000
Check Interval:     30 sec
Degraded Threshold: 2000 ms
```

### TCP Services (databases, Redis etc)

```
Name:               Postgres
URL / Address:      localhost:5432
```

### Saving a Profile

```
File → Save Profile  (Ctrl+S)
```

Saves all services and their config to a `.json` file.
The profile is auto-reloaded on next launch.

### Right-Click Menu

Right-click any service row:
- **Check Now** — trigger an immediate health check
- **Copy URL** — copy the service URL to clipboard
- **Remove** — stop monitoring and remove from list

## Tech Stack

| Component | Technology |
|-----------|-----------|
| UI Framework | Qt5 Widgets |
| Charts | Qt5 Charts (QChart, QLineSeries) |
| HTTP Checks | Qt5 Network (QNetworkAccessManager) |
| TCP Checks | Qt5 Network (QTcpSocket) |
| Build System | CMake 3.16+ with Ninja |
| JSON | Qt5 JSON (QJsonDocument) |
| Config | QSettings |
| Notifications | QSystemTrayIcon |
| Language | C++17 |

## Project Structure

```
devpulse/
├── CMakeLists.txt
├── README.md
└── src/
    ├── main.cpp
    ├── core/
    │   ├── Service.h
    │   ├── ServiceRepository.h / .cpp
    │   ├── MonitorEngine.h / .cpp
    │   ├── checkers/
    │   │   ├── IChecker.h
    │   │   ├── HttpChecker.h / .cpp
    │   │   ├── TcpChecker.h / .cpp
    │   │   └── CheckerFactory.h / .cpp
    │   ├── monitoring/
    │   │   ├── LatencyStore.h / .cpp
    │   │   └── UptimeTracker.h / .cpp
    │   └── storage/
    │       └── JsonProfileManager.h / .cpp
    ├── models/
    │   ├── ServiceTableModel.h / .cpp
    │   └── LogModel.h / .cpp
    └── ui/
        ├── MainWindow.h / .cpp
        ├── LatencyGraphWidget.h / .cpp
        ├── LogPanelWidget.h / .cpp
        ├── TrayManager.h / .cpp
        └── dialogs/
            └── AddServiceDialog.h / .cpp
```

## Author

Aman Negi
[github.com/Negi2110](https://github.com/Negi2110)
