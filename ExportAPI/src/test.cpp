#include <opencv2/opencv.hpp>
#include <Mesen/Mesen.h>
#include <ncurses.h>
#include <stdio.h>
#include <unistd.h>
#include <thread>

static struct DummyWindow {
} window_;

static struct DummyViewer {
} viewer_;

static std::string texture;
static std::mutex texture_mutex;

std::string image_to_ascii(uint32_t* buffer, uint32_t p_width, uint32_t p_height, int new_width = 80)
{
    // 1. 读取图像并转为灰度图
    cv::Mat img((int)p_height, (int)p_width, CV_8UC4, buffer);
    // 2. 计算新尺寸（保持宽高比，高度减半）
    int width = img.cols;
    int height = img.rows;
    double ratio = static_cast<double>(height) / width / 2.0; // 高度压缩一半
    int new_height = static_cast<int>(new_width * ratio);

    // 3. 调整图像大小
    cv::Mat resized_img;
    cv::resize(img, resized_img, cv::Size(new_width, new_height), 0, 0, cv::INTER_AREA);
    // cv::imwrite("tmp.jpg", resized_img);

    cv::Mat gray_image;
    cv::cvtColor(resized_img, gray_image, cv::COLOR_BGR2GRAY); // BGR转灰度

    // 4. 定义字符集（按视觉密度排序）
    const std::string ascii_chars = " ,-:!76CO$QHNM";

    // 5. 生成ASCII字符画
    std::string ascii_str;
    for (int y = 0; y < gray_image.rows; y++) {
        for (int x = 0; x < gray_image.cols; x++) {
            // 获取像素灰度值 (0-255)
            uchar pixel = gray_image.at<uchar>(y, x);

            int char_index = static_cast<int>(pixel / 255.0 * (ascii_chars.length() - 1));
            ascii_str += ascii_chars[char_index];

            // printf("%d %d %d\n", x, y, pixel);
        }
        ascii_str += '\n'; // 行尾换行
    }
    return ascii_str;
}

static void notify(int event, void* param)
{
    // std::cout << "Notify " << mesen_get_notification_type_name(event) <<  " Thread: " << std::this_thread::get_id() << std::endl;
    // printf("notify %s\n", mesen_get_notification_type_name(event));
    if (event == MESEN_NOTIFICATION_TYPE_REFRESH_SOFTWARE_RENDERER) {
        auto* renderer_frame = (MesenSoftwareRendererFrame*)param;
        auto frame = renderer_frame->frame;
        auto sz = image_to_ascii(frame.buffer, frame.width, frame.height, 65);
        {
            std::lock_guard lk(texture_mutex);
            texture = std::move(sz);
        }
        // std::cout << "\033[2J\033[1;1H";
        // printf("%s", sz.c_str());
    }
}

static void render_game() {
    {
        std::lock_guard lk(texture_mutex);
        printw(texture.c_str());
    }
}

int main(int argc, char* argv[])
{
    if (argc <= 1)
        return 0;
    initscr();
    curs_set(0);
    nodelay(stdscr, TRUE); 
    cbreak();
    noecho();
    keypad(stdscr, TRUE); 

    mesen_init();
    mesen_initialize_emu("./", &window_, &viewer_, true, true, false, true);

    mesen_set_default_config();
    // mesen_set_output_to_stdout(true);
    mesen_register_notification_callback(notify);
    bool succ = mesen_load_ROM(argv[1], NULL);

    if (succ) {
        // mesen_dump_ROM_info();

        // bool is_running = mesen_is_running();
        // printf("is_running?: %d\n", is_running);

        // bool is_paused = mesen_is_paused();
        // printf("is_paused?: %d\n", is_paused);

        // double aspect_ratio = mesen_get_aspect_ratio();
        // printf("aspect_ratio?: %lf\n", aspect_ratio);

        // sleep(100);
        // mesen_take_screenshot();
        // 
        
        int ch;
        bool running = true;
        
        while(running) {
            // 1. 处理输入
            while((ch = getch()) != ERR) {
                if(ch == 'q') running = false;
                // 其他输入处理...
            }
            
            // 3. 渲染
            clear();
            render_game();
            refresh();
            
            // 4. 控制帧率
            usleep(16666); // ≈60 FPS (1000000/60)
        }

    }

    mesen_release();

    endwin();
    return 0;

    // std::cout << "Main Thread: " << std::this_thread::get_id() << std::endl;
    // printf("load_ROM: %s\n", argv[1]);
    // printf("load_ROM succ?: %d\n", succ);

    // printf("release\n");
    // printf("release done\n");
    // return 0;
}