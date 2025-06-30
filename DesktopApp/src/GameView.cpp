#include "GameView.h"
#include "VirtualKey.h"
#include <Mesen/Mesen.h>
#include <QApplication>
#include <QKeyEvent>
#include <QSettings>
#include <set>

static std::map<int, QKeySequence> user_key_mapping;

using SuperSkillConfig = std::vector<std::set<int>>;
static SuperSkillConfig super_skill_config = { {
    { APP_VK_A },
    { APP_VK_A },
    { APP_VK_DOWN },
    { APP_VK_DOWN },

    { APP_VK_B, APP_VK_DOWN },
    { APP_VK_B, APP_VK_DOWN },
    { APP_VK_B, APP_VK_DOWN },
    { APP_VK_B, APP_VK_DOWN },
    { APP_VK_B, APP_VK_DOWN },
    { APP_VK_B, APP_VK_DOWN },
    { APP_VK_B, APP_VK_DOWN },
    { APP_VK_B, APP_VK_DOWN },
    { APP_VK_B, APP_VK_DOWN },
    { APP_VK_B, APP_VK_DOWN },
    { APP_VK_B, APP_VK_DOWN },
    { APP_VK_B, APP_VK_DOWN },
    { APP_VK_B, APP_VK_DOWN },
    { APP_VK_B, APP_VK_DOWN },
    { APP_VK_B, APP_VK_DOWN },
    { APP_VK_B, APP_VK_DOWN },
    { APP_VK_B, APP_VK_DOWN },
    { APP_VK_B, APP_VK_DOWN },
    { APP_VK_B },
} };

void GameView::refresh_key_mappings() {
    user_key_mapping.clear();

    QSettings settings;

    settings.beginGroup("Key Bindings");
    for (int vk = APP_VK_BEGIN; vk < APP_VK_END; vk++) {
        auto key_seq = QKeySequence(settings.value(vk_names[vk], default_keys[vk]).toString());
        user_key_mapping[vk] = key_seq;
    }

    settings.endGroup();
}

GameView::GameView(QWidget* parent)
    : QOpenGLWidget(parent)
    , canvas_width_(256)
    , canvas_height_(240)
    , using_super_skill_(false)
    , super_skill_frame_index_(0) {
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &GameView::update_frame);
    timer->start(16); // ~60 FPS

    setFocusPolicy(Qt::StrongFocus);
}

void GameView::update_frame() {
    QWidget* modalWidget = QApplication::activeModalWidget();
    if (modalWidget)
        return;

    update();

    if (using_super_skill_) {
        mesen_reset_key_state();
        for (auto& item : super_skill_config[super_skill_frame_index_++]) {
            mesen_set_key_state(item, true);
        }
        if (super_skill_frame_index_ == super_skill_config.size()) {
            super_skill_frame_index_ = 0;
        }
    }
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

void GameView::keyPressEvent(QKeyEvent* event) {
    if (!event->isAutoRepeat()) {
        QKeySequence seq(event->modifiers() | event->key());
        for (auto& item : user_key_mapping) {
            int vk = item.first;
            if (seq == item.second) {
                mesen_set_key_state(vk, true);
                if (vk == APP_VK_SUPER_SKILL) {
                    mesen_reset_key_state();
                    using_super_skill_ = true;
                    super_skill_frame_index_ = 0;
                }
            }
        }
        event->accept();
    }
}

void GameView::keyReleaseEvent(QKeyEvent* event) {
    if (!event->isAutoRepeat()) {
        for (auto& item : user_key_mapping) {
            int vk = item.first;
            if (event->key() == (item.second[0] & ~Qt::KeyboardModifierMask)) {
                mesen_set_key_state(vk, false);
                if (vk == APP_VK_SUPER_SKILL) {
                    using_super_skill_ = false;
                    mesen_reset_key_state();
                }
            }
        }
        event->accept();
    }
}