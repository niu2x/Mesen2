#include "main_window.h"

#include "game_view.h"
#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QFileDialog>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QSplitter>
#include <QStandardPaths>
#include <QStatusBar>
#include <QTextEdit>
#include <QTimer>

#include <Mesen/Mesen.h>

MainWindow* MainWindow::singleton_ = nullptr;

MainWindow::MainWindow()
    : QMainWindow() {

    singleton_ = this;

    QMenuBar* menu_bar = menuBar();

    QMenu* file_menu = menu_bar->addMenu("File");

    QAction* load_ROM_action = new QAction("Load ROM", this);
    file_menu->addAction(load_ROM_action);
    QObject::connect(load_ROM_action, &QAction::triggered, this, &MainWindow::load_ROM);

    QMenu* help_menu = menu_bar->addMenu("Help");
    QAction* about_action = new QAction("About", this);
    help_menu->addAction(about_action);
    QObject::connect(about_action, &QAction::triggered, []() {
        QMessageBox messageBox;
        messageBox.setText("这软件是完全免费~");
        messageBox.exec();
    });

    game_view_ = new GameView();
    log_view_ = new QTextEdit();

    QSplitter* splitter = new QSplitter(Qt::Vertical);
    splitter->addWidget(game_view_);
    splitter->addWidget(log_view_);

    log_view_->setFrameStyle(QFrame::Box);
    log_view_->setReadOnly(true);

    setCentralWidget(splitter);

    init_mesen();

    auto log_update_timer = new QTimer(this);
    log_update_timer->start(1000);
    QObject::connect(log_update_timer, &QTimer::timeout, this, &MainWindow::update_log_view);
}

QString MainWindow::get_app_data_dir() {
    QString app_data_path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(app_data_path);
    if (!dir.exists()) {
        dir.mkpath(app_data_path); // 递归创建目录
    }
    return app_data_path;
}

void MainWindow::update_log_view() {
    char log_buffer[1024];
    size_t log_length = mesen_get_log(log_buffer, 1024);
    log_buffer[log_length + 1] = 0;
    if (log_length) {
        log_view_->append(QString::fromUtf8(log_buffer, log_length));

        QTextCursor cursor = log_view_->textCursor();
        cursor.movePosition(QTextCursor::End);
        log_view_->setTextCursor(cursor);
        log_view_->ensureCursorVisible();
    }
}

void MainWindow::load_ROM() {
    QString filename = QFileDialog::getOpenFileName(this, // 父窗口
                                                    tr("Open File"), // 对话框标题
                                                    "", // 初始目录
                                                    tr("ROM Files (*.nes);;") // 文件过滤器
    );
    if (!filename.isEmpty()) {
        bool succ = mesen_load_ROM(filename.toUtf8().constData(), nullptr);
    }
}

MainWindow::~MainWindow() { mesen_release(); }

void MainWindow::init_mesen() {
    mesen_init();

    MesenPreferences preferences = {
        .HUD_size = MESEN_HUD_DISPLAY_SIZE_FIXED,
        .show_fps = false,
        .show_frame_counter = false,
        .show_game_timer = false,
        .show_debug_info = false,
        .disable_on_screen_display = true,
    };
    mesen_set_preferences(&preferences);
    mesen_set_output_to_stdout(false);

    MesenNesConfig nes_config = {
        .user_palette = &mesen_default_palette,
        .port_1 = {
            .key_mapping = {

            },
            .type = MESEN_CONTROLLER_TYPE_NES_CONTROLLER,
        }
    };
    mesen_set_NES_config(&nes_config);

    MesenVideoConfig video_config = {
        .video_filter = MESEN_VIDEO_FILTER_TYPE_NONE,
        .brightness = 0,
        .contrast = 0,
        .hue = 0,
        .saturation = 0,
    };
    mesen_set_video_config(&video_config);

    auto app_data_dir = get_app_data_dir();
    mesen_initialize_emu(app_data_dir.toUtf8().constData(), false, false, false);

    mesen_register_notification_callback(&MainWindow::mesen_notification_callback);
}

void MainWindow::mesen_notification_callback(MesenNotificationType noti_type, void* param) {
    if (noti_type == MESEN_NOTIFICATION_TYPE_REFRESH_SOFTWARE_RENDERER) {
        auto renderer_frame = (MesenSoftwareRendererFrame*)param;
        auto frame = renderer_frame->frame;
        singleton_->game_view_->set_frame_image(frame.buffer, frame.width, frame.height);
    }
}
