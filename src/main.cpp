#include "ui/mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("DevPulse");
    app.setApplicationVersion("0.1.0");
    app.setOrganizationName("devpulse");

    // Global dark theme
    app.setStyleSheet(R"(
        QMainWindow, QDialog, QWidget {
            background-color: #1e1e1e;
            color: #d4d4d4;
        }
        QMenuBar {
            background-color: #2d2d2d;
            color: #d4d4d4;
            border-bottom: 1px solid #3e3e3e;
        }
        QMenuBar::item:selected {
            background-color: #094771;
        }
        QMenu {
            background-color: #2d2d2d;
            color: #d4d4d4;
            border: 1px solid #3e3e3e;
        }
        QMenu::item:selected {
            background-color: #094771;
        }
        QTableView {
            background-color: #1e1e1e;
            color: #d4d4d4;
            gridline-color: #3e3e3e;
            border: none;
            selection-background-color: #094771;
        }
        QTableView::alternate-row {
            background-color: #252526;
        }
        QHeaderView::section {
            background-color: #2d2d2d;
            color: #d4d4d4;
            border: 1px solid #3e3e3e;
            padding: 4px;
            font-weight: bold;
        }
        QStatusBar {
            background-color: #007acc;
            color: white;
            font-size: 12px;
        }
        QStatusBar QLabel {
            color: white;
        }
        QLineEdit {
            background-color: #3c3c3c;
            color: #d4d4d4;
            border: 1px solid #555;
            border-radius: 3px;
            padding: 4px;
        }
        QLineEdit:focus {
            border: 1px solid #007acc;
        }
        QSpinBox {
            background-color: #3c3c3c;
            color: #d4d4d4;
            border: 1px solid #555;
            border-radius: 3px;
            padding: 4px;
        }
        QPushButton {
            background-color: #0e639c;
            color: white;
            border: none;
            border-radius: 3px;
            padding: 6px 16px;
            font-size: 13px;
        }
        QPushButton:hover {
            background-color: #1177bb;
        }
        QPushButton:disabled {
            background-color: #3c3c3c;
            color: #666;
        }
        QSplitter::handle {
            background-color: #3e3e3e;
        }
        QScrollBar:vertical {
            background-color: #1e1e1e;
            width: 10px;
        }
        QScrollBar::handle:vertical {
            background-color: #555;
            border-radius: 5px;
        }
        QLabel {
            color: #d4d4d4;
        }
        QFormLayout QLabel {
            color: #d4d4d4;
        }
    )");

    MainWindow w;
    w.show();
    return app.exec();
}
