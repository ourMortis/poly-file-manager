#include "mainwindow.hpp"

#include <QApplication>
#include <QStyleFactory>

namespace
{
const char *kDarkTheme = R"(
QWidget {
    background-color: #202020;
    color: #E8E8E8;
}
QMainWindow, QDialog { background-color: #202020; }

QMenuBar {
    background-color: #2B2B2B;
    color: #E8E8E8;
}
QMenuBar::item { background: transparent; padding: 4px 8px; }
QMenuBar::item:selected { background-color: #3A3A3A; }

QMenu {
    background-color: #2B2B2B;
    color: #E8E8E8;
    border: 1px solid #3A3A3A;
}
QMenu::item { padding: 4px 24px; }
QMenu::item:selected { background-color: #3A3A3A; }

QToolBar {
    background-color: #2B2B2B;
    border-bottom: 1px solid #3A3A3A;
    spacing: 4px;
}
QToolButton {
    background: transparent;
    color: #E8E8E8;
    padding: 4px 8px;
    border-radius: 4px;
}
QToolButton:hover { background-color: #3A3A3A; }
QToolButton:pressed { background-color: #4A4A4A; }

QLineEdit, QComboBox {
    background-color: #1E1E1E;
    color: #E8E8E8;
    border: 1px solid #3A3A3A;
    border-radius: 4px;
    padding: 3px 6px;
}
QLineEdit:focus, QComboBox:focus { border-color: #2D6AA0; }
QComboBox QAbstractItemView {
    background-color: #1E1E1E;
    color: #E8E8E8;
    selection-background-color: #2D6AA0;
    selection-color: #FFFFFF;
}

QPushButton {
    background-color: #3A3A3A;
    color: #E8E8E8;
    border: 1px solid #4A4A4A;
    border-radius: 4px;
    padding: 5px 14px;
}
QPushButton:hover { background-color: #4A4A4A; }
QPushButton:pressed { background-color: #555555; }
QPushButton:default { background-color: #2D6AA0; border-color: #2D6AA0; }

QListWidget {
    background-color: #1E1E1E;
    color: #E8E8E8;
    border: 1px solid #3A3A3A;
}
QListWidget::item { min-height: 24px; padding: 2px 6px; border: none; }
QListWidget::item:hover { background-color: #2A2D2E; }
QListWidget::item:selected { background-color: #2D6AA0; color: #FFFFFF; }
QListWidget::item:selected:hover { background-color: #3575AB; }
QListWidget#fileList {
    background-color: #1E1E1E;
    border: 1px solid #3A3A3A;
}
QListWidget#fileList::item { min-height: 24px; padding: 2px 6px; }
QListWidget#fileList::item:hover { background-color: #2A2D2E; }
QListWidget#fileList::item:selected { background-color: #2D6AA0; color: #FFFFFF; }
QListWidget#fileList::item:selected:hover { background-color: #3575AB; }

QTreeWidget {
    background-color: #1E1E1E;
    color: #E8E8E8;
    border: 1px solid #3A3A3A;
}
QTreeWidget::item { min-height: 24px; padding: 2px 4px; border: none; }
QTreeWidget::item:hover { background-color: #2A2D2E; }
QTreeWidget::item:selected { background-color: #2D6AA0; color: #FFFFFF; }

QTableWidget {
    background-color: #1E1E1E;
    color: #E8E8E8;
    gridline-color: #3A3A3A;
}
QTableWidget::item:selected { background-color: #2D6AA0; color: #FFFFFF; }
QHeaderView::section {
    background-color: #2B2B2B;
    color: #E8E8E8;
    border: 1px solid #3A3A3A;
    padding: 4px;
}

QStatusBar { background-color: #2B2B2B; color: #E8E8E8; }
QSplitter::handle { background-color: #2B2B2B; }
QToolTip { background-color: #2D2D2D; color: #E8E8E8; border: 1px solid #3A3A3A; }

QScrollBar:vertical { background: #1E1E1E; width: 12px; }
QScrollBar::handle:vertical { background: #4A4A4A; border-radius: 4px; min-height: 24px; }
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
QScrollBar:horizontal { background: #1E1E1E; height: 12px; }
QScrollBar::handle:horizontal { background: #4A4A4A; border-radius: 4px; min-width: 24px; }
QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0; }
)";
} // namespace

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    app.setApplicationName("PolyFileManager");
    app.setStyle(QStyleFactory::create("Fusion"));
    app.setStyleSheet(kDarkTheme);

    MainWindow window;
    window.show();
    return app.exec();
}
