#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/threading.h>
#include <vector>
#include <mutex>
#include <Mesen/Mesen.h>
#include <GLES2/gl2.h>

#include <stdint.h> // 确保 uint32_t 可用

// 将 RGBA (0xRRGGBBAA) 转换为 ABGR (0xAABBGGRR)
#define RGBA_TO_ABGR(color) ( \
    (((color) & 0xFF000000)) | (((color) & 0x00FF0000) >> 8) | (((color) & 0x0000FF00) << 0) | (((color) & 0x000000FF) << 16))

constexpr int TEXTURE_WIDTH = 256;
constexpr int TEXTURE_HEIGHT = 240;

static std::mutex texture_mutex;


const char* vertex_shader_source = R"(
attribute vec2 position;
attribute vec2 uv;
varying lowp vec2 v_uv;

void main() {
    gl_Position = vec4(position, 0.0, 1.0);
    v_uv = uv;
}
)";

const char* fragment_shader_source = R"(
varying lowp vec2 v_uv;
uniform sampler2D u_texture;

void main() {
    lowp vec4 texture_color = texture2D(u_texture, v_uv);
    gl_FragColor = texture_color;
}
)";

static GLuint compile_shader(GLenum type, const char* source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog(shader, 512, nullptr, info_log);
        emscripten_log(EM_LOG_ERROR, "Shader compilation failed: %s", info_log);
    }

    return shader;
}

static GLuint create_program(const char* vertex_source,
                             const char* fragment_source)
{
    GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_source);
    GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER,
                                            fragment_source);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetProgramInfoLog(program, 512, nullptr, info_log);
        emscripten_log(EM_LOG_ERROR, "Program linking failed: %s", info_log);
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return program;
}

struct Context {
    GLuint program;
    GLuint vbo;
    GLuint texture;
    std::vector<uint32_t> texture_buffer;
    bool running;
};

static Context my_context = { 0, 0, 0, {},  false };

struct Vertex {
    float x, y;
    float u, v;
};

static void setup()
{
    my_context.program = create_program(vertex_shader_source,
                                        fragment_shader_source);

    Vertex vertices[] = {
        { -1.0f, -1.0f, 0, 1 }, { 1.0f, -1.0f, 1, 1 }, { 1.0f, 1.0f, 1, 0 },
        { -1.0f, -1.0f, 0, 1 }, { 1.0f, 1.0f, 1, 0 },  { -1.0f, 1.0f, 0, 0 },
    };

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    my_context.vbo = vbo;

    my_context.texture_buffer.resize(TEXTURE_WIDTH * TEXTURE_HEIGHT);

    GLuint tex_id;

    glGenTextures(1, &tex_id);
    glBindTexture(GL_TEXTURE_2D, tex_id);

    // 设置纹理参数
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 传递纹理数据
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 TEXTURE_WIDTH,
                 TEXTURE_HEIGHT,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 my_context.texture_buffer.data());

    my_context.texture = tex_id;
}

static void cleanup()
{
    glDeleteBuffers(1, &my_context.vbo);
    glDeleteProgram(my_context.program);
}


static void render()
{
    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx = emscripten_webgl_get_current_context();
    emscripten_webgl_make_context_current(ctx);

    {
        std::lock_guard lk(texture_mutex);
        glBindTexture(GL_TEXTURE_2D, my_context.texture);
        glTexSubImage2D(GL_TEXTURE_2D,
                        0,
                        0,
                        0,
                        TEXTURE_WIDTH,
                        TEXTURE_HEIGHT,
                        GL_RGBA,
                        GL_UNSIGNED_BYTE,
                        my_context.texture_buffer.data());
    }

    glClear(GL_COLOR_BUFFER_BIT);
    glBindBuffer(GL_ARRAY_BUFFER, my_context.vbo);
    glUseProgram(my_context.program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, my_context.texture);

    GLint pos_location = glGetAttribLocation(my_context.program, "position");
    GLint uv_location = glGetAttribLocation(my_context.program, "uv");
    glEnableVertexAttribArray(pos_location);
    glEnableVertexAttribArray(uv_location);
    glVertexAttribPointer(
        pos_location, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glVertexAttribPointer(uv_location,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(Vertex),
                          (void*)(2 * sizeof(float)));

    GLint texture_location = glGetUniformLocation(my_context.program,
                                                  "u_texture");
    glUniform1i(texture_location, 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(pos_location);
    glDisableVertexAttribArray(uv_location);
}

EM_JS(void, notify_ready, (), {
    if (globalThis.nativeOnReady) {
        globalThis.nativeOnReady()
    }
});

EM_JS(void, set_title, (const char* version), {
    document.title = "Demo " + UTF8ToString(version, 32);
});

EM_BOOL key_callback(int eventType,
                     const EmscriptenKeyboardEvent* e,
                     void* userData)
{
    EM_BOOL handled = EM_FALSE;
    if (eventType == EMSCRIPTEN_EVENT_KEYDOWN
        || eventType == EMSCRIPTEN_EVENT_KEYUP) {

        bool pressed = eventType == EMSCRIPTEN_EVENT_KEYDOWN;
        switch (e->keyCode) {
            case 32:
            case 13:
            case 'A':
            case 'W':
            case 'D':
            case 'S':
            case 'K':
            case 'J':
            case 'U':
            case 'I': {
                handled = EM_TRUE;
                mesen_set_key_state(e->keyCode, pressed);
            }
        }
    }

    return handled;
}

int main()
{
    // if (emscripten_is_main_browser_thread()) {
    //     printf("运行在主线程（PROXY_TO_PTHREAD 未生效）\n");
    // } else {
    //     printf("运行在 Worker 线程（PROXY_TO_PTHREAD 已生效）\n");
    // }

    emscripten_set_canvas_element_size(
        "#canvas", TEXTURE_WIDTH, TEXTURE_HEIGHT);
    EmscriptenWebGLContextAttributes attrs;
    emscripten_webgl_init_context_attributes(&attrs);
    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx = emscripten_webgl_create_context(
        "#canvas", &attrs);
    emscripten_webgl_make_context_current(ctx);
    emscripten_set_keydown_callback(
        EMSCRIPTEN_EVENT_TARGET_DOCUMENT, NULL, EM_TRUE, key_callback);
    emscripten_set_keyup_callback(
        EMSCRIPTEN_EVENT_TARGET_DOCUMENT, NULL, EM_TRUE, key_callback);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    setup();

    notify_ready();

    emscripten_set_main_loop(render, 0, 1);
    cleanup();
    return 0;
}

static void notify(int noti, void* param)
{
    if (noti == MESEN_NOTIFICATION_TYPE_REFRESH_SOFTWARE_RENDERER) {
        auto* renderer_frame = (MesenSoftwareRendererFrame*)param;
        auto frame = renderer_frame->frame;

        {
            std::lock_guard lk(texture_mutex);
            if (frame.width == TEXTURE_WIDTH && frame.height == TEXTURE_HEIGHT) {
                for(int y = 0; y < frame.height; y ++) {
                    for(int x = 0; x < frame.width; x ++) {
                        my_context.texture_buffer[y*frame.width + x] = frame.buffer[y*frame.width + x];
                    }
                }
            }
            else {
                printf("texture size is: %d, %d\n", frame.width, frame.height);
            }
        }
    }
}


EMSCRIPTEN_KEEPALIVE
extern "C" void test(const char* ROM_path)
{
    mesen_init();
    mesen_initialize_emu("./", true, false, false);

    MesenNesConfig NES_config = {
        .user_palette = { RGBA_TO_ABGR(0xFF666666), RGBA_TO_ABGR(0xFF002A88), RGBA_TO_ABGR(0xFF1412A7), RGBA_TO_ABGR(0xFF3B00A4), RGBA_TO_ABGR(0xFF5C007E), RGBA_TO_ABGR(0xFF6E0040), RGBA_TO_ABGR(0xFF6C0600), RGBA_TO_ABGR(0xFF561D00), RGBA_TO_ABGR(0xFF333500), RGBA_TO_ABGR(0xFF0B4800), RGBA_TO_ABGR(0xFF005200), RGBA_TO_ABGR(0xFF004F08), RGBA_TO_ABGR(0xFF00404D), RGBA_TO_ABGR(0xFF000000), RGBA_TO_ABGR(0xFF000000), RGBA_TO_ABGR(0xFF000000), RGBA_TO_ABGR(0xFFADADAD), RGBA_TO_ABGR(0xFF155FD9), RGBA_TO_ABGR(0xFF4240FF), RGBA_TO_ABGR(0xFF7527FE), RGBA_TO_ABGR(0xFFA01ACC), RGBA_TO_ABGR(0xFFB71E7B), RGBA_TO_ABGR(0xFFB53120), RGBA_TO_ABGR(0xFF994E00), RGBA_TO_ABGR(0xFF6B6D00), RGBA_TO_ABGR(0xFF388700), RGBA_TO_ABGR(0xFF0C9300), RGBA_TO_ABGR(0xFF008F32), RGBA_TO_ABGR(0xFF007C8D), RGBA_TO_ABGR(0xFF000000), RGBA_TO_ABGR(0xFF000000), RGBA_TO_ABGR(0xFF000000), RGBA_TO_ABGR(0xFFFFFEFF), RGBA_TO_ABGR(0xFF64B0FF), RGBA_TO_ABGR(0xFF9290FF), RGBA_TO_ABGR(0xFFC676FF), RGBA_TO_ABGR(0xFFF36AFF), RGBA_TO_ABGR(0xFFFE6ECC), RGBA_TO_ABGR(0xFFFE8170), RGBA_TO_ABGR(0xFFEA9E22), RGBA_TO_ABGR(0xFFBCBE00), RGBA_TO_ABGR(0xFF88D800), RGBA_TO_ABGR(0xFF5CE430), RGBA_TO_ABGR(0xFF45E082), RGBA_TO_ABGR(0xFF48CDDE), RGBA_TO_ABGR(0xFF4F4F4F), RGBA_TO_ABGR(0xFF000000), RGBA_TO_ABGR(0xFF000000), RGBA_TO_ABGR(0xFFFFFEFF), RGBA_TO_ABGR(0xFFC0DFFF), RGBA_TO_ABGR(0xFFD3D2FF), RGBA_TO_ABGR(0xFFE8C8FF), RGBA_TO_ABGR(0xFFFBC2FF), RGBA_TO_ABGR(0xFFFEC4EA), RGBA_TO_ABGR(0xFFFECCC5), RGBA_TO_ABGR(0xFFF7D8A5), RGBA_TO_ABGR(0xFFE4E594), RGBA_TO_ABGR(0xFFCFEF96), RGBA_TO_ABGR(0xFFBDF4AB), RGBA_TO_ABGR(0xFFB3F3CC), RGBA_TO_ABGR(0xFFB5EBF2), RGBA_TO_ABGR(0xFFB8B8B8), RGBA_TO_ABGR(0xFF000000), RGBA_TO_ABGR(0xFF000000) },
        .port_1 = {
            .key_mapping = {
                .A = 'K',
                .B = 'J',

                .up = 'W',
                .down = 'S',
                .left = 'A',
                .right = 'D',

                .start = 13,
                .select = 32,

                .turbo_A = 'I',
                .turbo_B = 'U',

                .turbo_speed = 2,
            },
            .type = MESEN_CONTROLLER_TYPE_NES_CONTROLLER,
        },
    };
    mesen_set_NES_config(&NES_config);
    mesen_register_notification_callback(notify);
    bool succ = mesen_load_ROM(ROM_path, NULL);
    printf("load ROM succ?: %d\n", succ);
    my_context.running = succ;
}


