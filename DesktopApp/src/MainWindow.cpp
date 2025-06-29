#include "MainWindow.h"

#include "GameView.h"

#include <Mesen/Mesen.h>
#include <QAction>
#include <QApplication>
#include <QAudioOutput>
#include <QBuffer>
#include <QDebug>
#include <QFileDialog>
#include <QKeyEvent>
#include <QLabel>
#include <QMessageBox>
#include <QSettings>
#include <QSplitter>
#include <QStandardPaths>
#include <QStatusBar>
#include <QTabWidget>
#include <QTextEdit>
#include <QTimer>
#include <QToolBar>

#include "ControlsSettingDialog.h"
#include "VirtualKey.h"

static std::map<uint32_t, int> user_key_mapping;

MainWindow* MainWindow::singleton_ = nullptr;

void MainWindow::init_menu_bar() {
    QMenuBar* menu_bar = menuBar();

    QMenu* file_menu = menu_bar->addMenu("File");

    QAction* load_ROM_action = new QAction("Load ROM", this);
    file_menu->addAction(load_ROM_action);
    QObject::connect(load_ROM_action, &QAction::triggered, this, &MainWindow::load_ROM);

    recent_games_menu_ = file_menu->addMenu("Recent Games");
    QObject::connect(
        recent_games_menu_, &QMenu::aboutToShow, this, &MainWindow::build_recent_games_menu);

    QMenu* settings_menu = menu_bar->addMenu("Settings");
    QAction* controls_setting = new QAction("Controls Setting");
    settings_menu->addAction(controls_setting);
    QObject::connect(controls_setting, &QAction::triggered, [this]() {
        ControlsSettingDialog dialog(this);
        dialog.exec();
    });

    QMenu* help_menu = menu_bar->addMenu("Help");
    QAction* about_action = new QAction("About", this);
    help_menu->addAction(about_action);
    QObject::connect(about_action, &QAction::triggered, []() {
        QMessageBox messageBox;
        messageBox.setText("这软件是完全免费~");
        messageBox.exec();
    });
}

void MainWindow::init_ui() {
    game_view_ = new GameView();
    log_view_ = new QTextEdit();

    QTabWidget* tabs = new QTabWidget(this);
    tabs->addTab(game_view_, "Game");
    tabs->addTab(log_view_, "Log");

    log_view_->setFrameStyle(QFrame::Box);
    log_view_->setReadOnly(true);

    setCentralWidget(tabs);

    auto log_update_timer = new QTimer(this);
    log_update_timer->start(1000);
    QObject::connect(log_update_timer, &QTimer::timeout, this, &MainWindow::update_log_view);

    game_sound_device_ = new GameSoundDevice(this);
}

void MainWindow::init_settings() { }

MainWindow::MainWindow()
    : QMainWindow()
    , audio_output_(nullptr) {

    singleton_ = this;

    init_menu_bar();
    init_tools_bar();
    init_ui();

    init_mesen();

    refresh_key_mappings();
}

void MainWindow::init_tools_bar() {
    auto tools_bar = new QToolBar();
    addToolBar(tools_bar);

    auto reset_btn = new QAction(QIcon(":/icon/reset.png"), "Reset");
    tools_bar->addAction(reset_btn);
    QObject::connect(reset_btn, &QAction::triggered, []() {
        MesenExecuteShortcutParams shortcut = { .action = MESEN_SHORTCUT_TYPE_EXEC_RESET };
        mesen_execute_shortcut(&shortcut);
    });
}

void MainWindow::start_audio_device(uint32_t buffer_size, bool is_stereo, uint32_t sample_rate) {

    QCoreApplication* app = QCoreApplication::instance();
    QMetaObject::invokeMethod(
        app,
        [this, sample_rate, is_stereo, buffer_size]() {
            if (!audio_output_) {
                audio_format_.setSampleRate(sample_rate);
                audio_format_.setChannelCount(is_stereo ? 2 : 1);
                audio_format_.setSampleSize(16);
                audio_format_.setCodec("audio/pcm");
                audio_format_.setByteOrder(QAudioFormat::LittleEndian);
                audio_format_.setSampleType(QAudioFormat::SignedInt);

                game_sound_device_->start();

                audio_output_ = new QAudioOutput(audio_format_, this);
                audio_output_->setBufferSize(buffer_size);

                audio_output_->start(game_sound_device_);
            }
        },
        Qt::QueuedConnection);
}

void MainWindow::stop_audio_device() {

    QCoreApplication* app = QCoreApplication::instance();
    QMetaObject::invokeMethod(
        app,
        [this]() {
            if (audio_output_) {
                // 停止音频
                audio_output_->stop();

                delete audio_output_;
                audio_output_ = nullptr; 

                game_sound_device_->stop();
            }
        },
        Qt::QueuedConnection);
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
                                                    tr("Select A ROM File"), // 对话框标题
                                                    "", // 初始目录
                                                    tr("ROM Files (*.nes);;") // 文件过滤器
    );
    if (!filename.isEmpty()) {
        bool succ = mesen_load_ROM(filename.toUtf8().constData(), nullptr);
    }
}

MainWindow::~MainWindow() { mesen_release(); }

void MainWindow::closeEvent(QCloseEvent* event) {
    stop_audio_device();
    event->accept();
}

void MainWindow::build_recent_games_menu() {
    recent_games_menu_->clear();
    QDir dir(mesen_get_recent_games_folder());

    if (!dir.exists()) {
        return;
    }

    for (const QFileInfo& file : dir.entryInfoList(QDir::Files)) {
        QAction* action = recent_games_menu_->addAction(file.fileName());
        QObject::connect(action, &QAction::triggered, [file]() {
            auto path = file.absoluteFilePath();
            mesen_load_recent_game(path.toUtf8().constData(), false);
        });
    }
}

void MainWindow::refresh_key_mappings() {
    user_key_mapping.clear();

    QSettings settings;

    settings.beginGroup("Key Bindings");
    for (int vk = APP_VK_BEGIN; vk < APP_VK_END; vk++) {

        auto key_seq = QKeySequence(settings.value(vk_names[vk], default_keys[vk]).toString());
        auto key = static_cast<Qt::Key>(key_seq[0]);
        user_key_mapping[key] = vk;
    }

    settings.endGroup();
}

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
                .A = APP_VK_A,
                .B = APP_VK_B,
                .up = APP_VK_UP,
                .down = APP_VK_DOWN,
                .left = APP_VK_LEFT,
                .right = APP_VK_RIGHT,
                .start = APP_VK_START,
                .select = APP_VK_SELECT,
                .turbo_A = APP_VK_TURBO_A,
                .turbo_B = APP_VK_TURBO_B,
                .turbo_speed = 3,
            },
            .type = MESEN_CONTROLLER_TYPE_NES_CONTROLLER,
        },
    };
    mesen_set_NES_config(&nes_config);

    MesenShortcutKeyInfo shortcut_infos[1];

    shortcut_infos[0] = {
        .shortcut = MESEN_SHORTCUT_TYPE_REWIND,
        .key_combination = {
            .keys = {
                APP_VK_REWIND, 0, 0,
            },
        },
    };

    mesen_set_shortcut_keys(shortcut_infos, 1);

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
    } else if (noti_type == MESEN_NOTIFICATION_TYPE_START_AUDIO_DEVICE) {
        auto* device = (MesenAudioDeviceParam*)param;
        singleton_->start_audio_device(device->buffer_size, device->is_stereo, device->sample_rate);

    } else if (noti_type == MESEN_NOTIFICATION_TYPE_STOP_AUDIO_DEVICE) {
        singleton_->stop_audio_device();
    }
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    if (!event->isAutoRepeat()) {
        auto it = user_key_mapping.find(event->key());
        if (it != user_key_mapping.end()) {
            mesen_set_key_state(it->second, true);
            event->accept(); // 表示事件已处理
        }
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent* event) {
    if (!event->isAutoRepeat()) {
        auto it = user_key_mapping.find(event->key());
        if (it != user_key_mapping.end()) {
            mesen_set_key_state(it->second, false);
            event->accept(); // 表示事件已处理
        }
    }
}