#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/threading.h>
#include <vector>
#include <mutex>
#include <thread>
#include <Mesen/Mesen.h>
#include <GLES2/gl2.h>

#include <stdint.h> // 确保 uint32_t 可用

// 将 RGBA (0xRRGGBBAA) 转换为 ABGR (0xAABBGGRR)
#define RGBA_TO_ABGR(color) ( \
    (((color) & 0xFF000000)) | (((color) & 0x00FF0000) >> 8) | (((color) & 0x0000FF00) << 0) | (((color) & 0x000000FF) << 16))

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

    uint32_t tex_width, tex_height;
    GLuint texture;

    std::vector<uint32_t> texture_buffer;
    uint32_t buf_tex_width, buf_tex_height;

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

    my_context.tex_width = my_context.tex_height = 0;
    my_context.buf_tex_width = my_context.buf_tex_height = 0;
}

static void destroy_texture()
{
    if (my_context.texture) {
        glDeleteTextures(1, &my_context.texture);
        my_context.texture = 0;
    }
}

static void create_texture(uint32_t w, uint32_t h)
{
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
        w,
        h,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        my_context.texture_buffer.data());

    my_context.texture = tex_id;
    my_context.tex_width = w;
    my_context.tex_height = h;
}

static void render()
{
    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx = emscripten_webgl_get_current_context();
    emscripten_webgl_make_context_current(ctx);

    if (my_context.tex_width != my_context.buf_tex_width
        || my_context.tex_height != my_context.buf_tex_height) {
        destroy_texture();
        create_texture(my_context.buf_tex_width, my_context.buf_tex_height);
    }

    {
        std::lock_guard lk(texture_mutex);
        glBindTexture(GL_TEXTURE_2D, my_context.texture);
        glTexSubImage2D(GL_TEXTURE_2D,
            0,
            0,
            0,
            my_context.tex_width,
            my_context.tex_height,
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
    return 0;
}

static void notify(int noti, void* param)
{
    if (noti == MESEN_NOTIFICATION_TYPE_REFRESH_SOFTWARE_RENDERER) {
        auto* renderer_frame = (MesenSoftwareRendererFrame*)param;

        {
            std::lock_guard lk(texture_mutex);

            auto frame = renderer_frame->frame;
            auto emulator_HUD = renderer_frame->emulator_HUD;

            if (frame.width != my_context.buf_tex_width
                || frame.height != my_context.buf_tex_height) {
                my_context.texture_buffer.resize(frame.width * frame.height);
                my_context.buf_tex_width = frame.width;
                my_context.buf_tex_height = frame.height;
            }

            for (int y = 0; y < frame.height; y++) {
                for (int x = 0; x < frame.width; x++) {
                    my_context.texture_buffer[y * frame.width + x]
                        = frame.buffer[y * frame.width + x];
                }
            }

            int hud_offset_y = frame.height - emulator_HUD.height;

            for (int y = 0; y < emulator_HUD.height; y++) {
                for (int x = 0; x < emulator_HUD.width; x++) {
                    my_context.texture_buffer[(y + hud_offset_y) * frame.width + x]
                        |= emulator_HUD.buffer[y * emulator_HUD.width + x];
                }
            }
        }
    }
}

std::once_flag emulator_init;


EMSCRIPTEN_KEEPALIVE
extern "C" void load_NES_file(const char* ROM_path)
{
    std::call_once(emulator_init, []() {
        mesen_init();
        mesen_initialize_emu("./", true, false, false);
        mesen_register_notification_callback(notify);

        MesenVideoConfig video_config {
            .video_filter = MESEN_VIDEO_FILTER_TYPE_HQ_2X,
            .brightness = 0,
            .contrast = 0,
            .hue = 0,
            .saturation = 0,
        };
        mesen_set_video_config(&video_config);

        MesenPreferences preferences {
            .HUD_size = MESEN_HUD_DISPLAY_SIZE_SCALED,
            .show_fps = false,
            .show_frame_counter = false,
            .show_game_timer = false,
            .show_debug_info = false,
        };
        mesen_set_preferences(&preferences);
    });

    MesenNesConfig NES_config = {
        .user_palette = {},
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

    for (int i = 0; i < 512; i++) {
        NES_config.user_palette[i] = RGBA_TO_ABGR(mesen_default_palette[i]);
    }

    mesen_set_NES_config(&NES_config);

    bool succ = mesen_load_ROM(ROM_path, NULL);
    printf("load ROM succ?: %d\n", succ);
    my_context.running = succ;
}

EMSCRIPTEN_KEEPALIVE 
extern "C" void reset_emulator() {
    MesenExecuteShortcutParams exec_params {
        .shortcut = MESEN_SHORTCUT_EXEC_RESET,
    };
    mesen_execute_shortcut(&exec_params);
}