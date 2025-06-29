#include <QApplication>
#include <QDesktopWidget>
#include <QStyle>

#include "MainWindow.h"

static void center_window(QWidget* window, QApplication* app) {
    window->setGeometry(QStyle::alignedRect(
        Qt::LeftToRight, Qt::AlignCenter, window->size(), app->desktop()->availableGeometry()));
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    MainWindow main_window;
    center_window(&main_window, &app);

    main_window.show();

    return app.exec();
}