#pragma once

#include <QApplication>
#include <QAudioOutput>
#include <QMainWindow>
#include <QMenuBar>
#include <QTextEdit>

#include <Mesen/Mesen.h>

#include "GameSoundDevice.h"
#include "GameView.h"

class MainWindow : public QMainWindow {
public:
    MainWindow();
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    void init_mesen();
    void init_menu_bar();
    void update_log_view();
    void load_ROM();

    QTextEdit* log_view_;
    GameView* game_view_;
    QMenu* recent_games_menu_;

    QAudioFormat audio_format_;
    QAudioOutput* audio_output_;
    GameSoundDevice* game_sound_device_;

    QString get_app_data_dir();

    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

    void build_recent_games_menu();

    void init_tools_bar();
    void init_ui();

    void init_settings();
    void refresh_key_mappings();

    void start_audio_device(uint32_t buffer_size, bool is_stereo, uint32_t sample_rate);
    void stop_audio_device();

    static void mesen_notification_callback(MesenNotificationType event_type, void* param);
    static MainWindow* singleton_;
};