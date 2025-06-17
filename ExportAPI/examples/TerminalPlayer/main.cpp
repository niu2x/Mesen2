#include <opencv2/opencv.hpp>

#include <Mesen/Mesen.h>
#include <getopt.h>
#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <thread>
#include <unistd.h>
#define USE_NCURSES 1

static std::string texture;
static std::mutex texture_mutex;

static uint8_t gray[1024][1024];
static int tex_width = 0, tex_height = 0;
static int screen_width = 100;
static bool use_ascii = true;
static bool use_edge = false;

void image_to_ascii(uint32_t* buffer, uint32_t p_width, uint32_t p_height, int new_width = 80)
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

    if (use_edge) {
        cv::Mat edges;
        Canny(gray_image, edges, 50, 150); // 阈值可调整
        gray_image = edges;
    }

    // 4. 定义字符集（按视觉密度排序）
    const std::string ascii_chars = " ,-:!76CO$QHNM";

    // 5. 生成ASCII字符画
    {
        std::lock_guard lk(texture_mutex);
        // std::string ascii_str;
        texture.resize(1024 * 1024);
        int tex_index = 0;

        tex_width = gray_image.cols;
        tex_height = gray_image.rows;

        for (int y = 0; y < gray_image.rows; y++) {
            for (int x = 0; x < gray_image.cols; x++) {
                // 获取像素灰度值 (0-255)
                uchar pixel = gray_image.at<uchar>(y, x);
                if (use_ascii) {
                    int char_index = static_cast<int>(pixel / 255.0 * (ascii_chars.length() - 1));
                    texture[tex_index++] = ascii_chars[char_index];
                } else {
                    gray[y][x] = pixel;
                }
            }
            texture[tex_index++] = '\n'; // 行尾换行
        }
    }
}

static void notify(int event, void* param)
{
    if (event == MESEN_NOTIFICATION_TYPE_REFRESH_SOFTWARE_RENDERER) {
        auto* renderer_frame = (MesenSoftwareRendererFrame*)param;
        auto frame = renderer_frame->frame;
        image_to_ascii(frame.buffer, frame.width, frame.height, screen_width);
    }
}

static void render_game()
{
    if (use_ascii) {
        std::lock_guard lk(texture_mutex);
        printw(texture.c_str());
    } else {
        std::lock_guard lk(texture_mutex);
        for (int y = 0; y < tex_height; y++) {
            move(y, 0);
            for (int x = 0; x < tex_width; x++) {
                int level = gray[y][x] / 4;
                attron(COLOR_PAIR(level));
                printw(" ");
                attroff(COLOR_PAIR(level));
            }
        }
    }
}

int main(int argc, char* argv[])
{
    int opt;

    while ((opt = getopt(argc, argv, "a:e:w:")) != -1) {
        switch (opt) {
        case 'a':
            use_ascii = strcmp(optarg, "t") == 0;
            break;

        case 'e':
            use_edge = strcmp(optarg, "t") == 0;
            break;

        case 'w':
            screen_width = atoi(optarg);
            break;
        case '?':
            // getopt already printed an error message
            exit(1);
            break;
        default:
            printf("Unexpected case in getopt\n");
            exit(1);
            break;
        }
    }

    // Print remaining arguments (non-options)
    if (optind >= argc) {
        printf("no nes.\n");
        exit(1);
    }

#if USE_NCURSES
    initscr();
    curs_set(0);
    nodelay(stdscr, TRUE);
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    if (!use_ascii) {

        start_color();

        for (int i = 0; i < 64; i++) {
            init_color(i, i * 14, i * 14, i * 14);
            init_pair(i, i, i);
        }
    }

#endif

    mesen_init();
    mesen_initialize_emu("./", true, false, false);

    MesenNesConfig NES_config = {
        .user_palette = {},
        .port_1 = {
            .key_mapping = {
                .A = 'J',
                .B = 'K',

                .up = 'W',
                .down = 'S',
                .left = 'A',
                .right = 'D',

                .start = 'M',
                .select = 'N',

                .turbo_A = 'U',
                .turbo_B = 'I',

                .turbo_speed = 0,
            },
            .type = MESEN_CONTROLLER_TYPE_NES_CONTROLLER,
        },
    };
    memcpy(NES_config.user_palette, mesen_default_palette, sizeof(MesenPalette));
    mesen_set_NES_config(&NES_config);
    // mesen_set_output_to_stdout(true);
    mesen_register_notification_callback(notify);
    bool succ = mesen_load_ROM(argv[optind], NULL);

    if (succ) {
        int ch;
        bool running = true;

        while (running) {
            mesen_reset_key_state();
            // 1. 处理输入
            while ((ch = getch()) != ERR) {
                if (ch == 'q')
                    running = false;
                else if (ch == 'm') {
                    mesen_set_key_state('M', true);
                } else if (ch == 'n') {
                    mesen_set_key_state('N', true);
                } else if (ch == 'w') {
                    mesen_set_key_state('W', true);
                } else if (ch == 's') {
                    mesen_set_key_state('S', true);
                } else if (ch == 'a') {
                    mesen_set_key_state('A', true);
                } else if (ch == 'd') {
                    mesen_set_key_state('D', true);
                }
                else if (ch == 'j') {
                    mesen_set_key_state('J', true);
                }
                else if (ch == 'k') {
                    mesen_set_key_state('K', true);
                }

                else if (ch == 'u') {
                    mesen_set_key_state('U', true);
                } else if (ch == 'i') {
                    mesen_set_key_state('I', true);
                }
            }

            // 3. 渲染
#if USE_NCURSES
            clear();
            render_game();
            refresh();
#endif

            // 4. 控制帧率
            usleep(16666 * 4); // ≈60 FPS (1000000/60)
        }
    }

    mesen_release();

#if USE_NCURSES
    endwin();
#endif
    return 0;
}