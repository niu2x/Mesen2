#include "game_view.h"
#include <QApplication>

GameView::GameView(QWidget* parent)
    : QOpenGLWidget(parent)
    , canvas_width_(256)
    , canvas_height_(240) {
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &GameView::update_frame);
    timer->start(16); // ~60 FPS
}

void GameView::update_frame() {
    QWidget* modalWidget = QApplication::activeModalWidget();
    if (modalWidget)
        return;

    update();
}

void GameView::set_frame_image(uint32_t* buffer, uint32_t width, uint32_t height) {
    std::lock_guard lk(image_mutex_);
    if (image_.format() != QImage::Format_ARGB32 || image_.width() != static_cast<int>(width)
        || image_.height() != static_cast<int>(height)) {
        image_ = QImage(width, height, QImage::Format_ARGB32);
        canvas_width_ = width;
        canvas_height_ = height;
    }

    // 直接拷贝内存
    memcpy(image_.bits(), buffer, width * height * sizeof(uint32_t));
}

void GameView::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w, h, 0, -1, 1); // 设置2D正交投影
}

void GameView::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void GameView::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT);
    std::lock_guard lk(image_mutex_);
    if (!image_.isNull()) {

        float widgetAspect = float(width()) / height();
        float imageAspect = float(image_.width()) / image_.height();

        float scale, drawWidth, drawHeight;
        float xOffset = 0, yOffset = 0;

        if (widgetAspect > imageAspect) {
            // 窗口比图像宽，按高度缩放
            scale = float(height()) / image_.height();
            drawWidth = image_.width() * scale;
            drawHeight = height();
            xOffset = (width() - drawWidth) / 2.0f;
        } else {
            // 窗口比图像高，按宽度缩放
            scale = float(width()) / image_.width();
            drawHeight = image_.height() * scale;
            drawWidth = width();
            yOffset = (height() - drawHeight) / 2.0f;
        }

        // 保存当前矩阵
        glPushMatrix();

        // 移动到绘制位置
        glTranslatef(xOffset, yOffset, 0);

        // 缩放图像
        glPixelZoom(drawWidth / image_.width(), -drawHeight / image_.height());

        // 绘制图像
        glRasterPos2f(0, 0);
        glDrawPixels(image_.width(), image_.height(), GL_BGRA, GL_UNSIGNED_BYTE, image_.bits());

        // 恢复矩阵
        glPopMatrix();
    }
}