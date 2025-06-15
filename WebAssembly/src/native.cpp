#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <vector>
#include <Mesen/Mesen.h>
#include <GLES2/gl2.h>

const int TEXTURE_WIDTH = 256;
const int TEXTURE_HEIGHT = 240;

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
    std::vector<uint8_t> texture_buffer;
    double time;
    // std::unique_ptr<windique::Emulator> emulator;
    // std::unique_ptr<WebGLRenderer> renderer;
    // windique::GamepadState gamepad_state;
    bool turbo_a;
    bool turbo_b;
    uint8_t frames;

    bool running;
};

static Context my_context = { 0, 0, 0, {}, 0, /*nullptr, false,*/ false, false, 0, false };

struct Vertex {
    float x, y;
    float u, v;
};

static void setup()
{
    my_context.time = 0;
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

    my_context.texture_buffer.resize(TEXTURE_WIDTH * TEXTURE_HEIGHT * 3);

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

static void update_canvas()
{
    my_context.frames++;
    if (my_context.running) {
        // auto em = my_context.emulator.get();
        // auto the_ks = my_context.gamepad_state;
        // if (my_context.frames & 1) {
        //     if (my_context.turbo_a) {
        //         the_ks.A = true;
        //     }

        //     if (my_context.turbo_b) {
        //         the_ks.B = true;
        //     }
        // }
        // em->set_gamepad_state(the_ks, {});
        // em->kick_one_frame();
    }
}

static void render()
{
    my_context.time += 1.0 / 60;

    update_canvas();

    // auto* fb = my_context.emulator->get_frame_buffer();
    // if (fb) {
    //     glBindTexture(GL_TEXTURE_2D, my_context.texture);
    //     glTexSubImage2D(GL_TEXTURE_2D,
    //                     0,
    //                     0,
    //                     0,
    //                     TEXTURE_WIDTH,
    //                     TEXTURE_HEIGHT,
    //                     GL_RGBA,
    //                     GL_UNSIGNED_BYTE,
    //                     fb);
    // }

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
            case 32: {
                // my_context.gamepad_state.select = pressed;
                handled = EM_TRUE;
                break;
            }
            case 13: {
                // my_context.gamepad_state.start = pressed;
                handled = EM_TRUE;
                break;
            }
            case 'A': {
                // my_context.gamepad_state.left = pressed;
                handled = EM_TRUE;
                break;
            }
            case 'W': {
                // my_context.gamepad_state.up = pressed;
                handled = EM_TRUE;
                break;
            }
            case 'D': {
                // my_context.gamepad_state.right = pressed;
                handled = EM_TRUE;
                break;
            }
            case 'S': {
                // my_context.gamepad_state.down = pressed;
                handled = EM_TRUE;
                break;
            }
            // case 'U': {
            //     my_context.gamepad_state.B = pressed;
            //     handled = EM_TRUE;
            //     break;
            // }
            case 'K': {
                // my_context.gamepad_state.A = pressed;
                handled = EM_TRUE;
                break;
            }
            case 'J': {
                // my_context.gamepad_state.B = pressed;
                // my_context.turbo_b = pressed;
                handled = EM_TRUE;
                break;
            }
                // case 'I': {
                //     my_context.turbo_a = pressed;
                //     handled = EM_TRUE;
                //     break;
                // }
        }
    }

    return handled;
}

int main()
{
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

static void init_emulator()
{
    // if (!my_context.emulator) {
        // printf("init_emulator\n");
        // my_context.emulator = std::make_unique<windique::Emulator>();
    // }
}

EMSCRIPTEN_KEEPALIVE
extern "C" void load_nes(const uint8_t* nes, size_t len)
{
    init_emulator();

    // my_context.emulator->set_pixel_format(windique::PixelFormat::R8_G8_B8_A8);
    // bool succ = my_context.emulator->load_iNES(nes, len, "");
    // if (succ) {
    //     my_context.running = true;
    // } else {
    //     printf("load nes: %s\n", succ ? "succ" : "fail");
    //     my_context.emulator.reset();
    // }
}


EMSCRIPTEN_KEEPALIVE
extern "C" void test()
{
    mesen_init();
}


