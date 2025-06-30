#pragma once
#include <QImage>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QTimer>
#include <mutex>

class GameView : public QOpenGLWidget, protected QOpenGLFunctions {
public:
    GameView(QWidget* parent = nullptr);
    void set_frame_image(uint32_t* buffer, uint32_t width, uint32_t height);

    void refresh_key_mappings();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;

    void paintGL() override;

    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private:
    void update_frame();

private:
    QImage image_;
    uint32_t canvas_width_;
    uint32_t canvas_height_;
    std::mutex image_mutex_;
};