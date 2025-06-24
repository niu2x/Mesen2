#include <QAction>
#include <QApplication>
#include <QLabel>
#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QSplitter>
#include <QStatusBar>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    
    QMainWindow main_window;
    QMenuBar *menu_bar = main_window.menuBar();

    QMenu *file_menu = menu_bar->addMenu("File");

    QAction *exit_action = new QAction("Exit", &main_window);
    file_menu->addAction(exit_action);
    QObject::connect(exit_action, &QAction::triggered, &app, &QApplication::quit);

    QMenu* help_menu = menu_bar->addMenu("Help");
    QAction* about_action = new QAction("About", &main_window);
    help_menu->addAction(about_action);
    QObject::connect(about_action, &QAction::triggered, []() {
        QMessageBox messageBox;
        messageBox.setText("这软件是完全免费~");
        messageBox.exec();
    });

    auto label_1 = new QLabel("1");
    auto label_2 = new QLabel("2");

    auto central_widget = new QWidget;
    QSplitter* splitter = new QSplitter(Qt::Vertical, central_widget);
    splitter->addWidget(label_1);
    splitter->addWidget(label_2);

    label_1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    label_2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    splitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    main_window.setCentralWidget(central_widget);

    auto status_bar = main_window.statusBar();
    status_bar->showMessage("launch success.");

    main_window.show();

    return app.exec();
}