#pragma once

#include "game_view.h"
#include <Mesen/Mesen.h>
#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QTextEdit>

class MainWindow : public QMainWindow {
public:
    MainWindow();
    ~MainWindow();

private:
    void init_mesen();
    void update_log_view();
    void load_ROM();

    QTextEdit* log_view_;
    GameView* game_view_;
    QMenu* recent_games_menu_;

    QString get_app_data_dir();

    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

    void build_recent_games_menu();

    static void mesen_notification_callback(MesenNotificationType event_type, void* param);
    static MainWindow* singleton_;
};