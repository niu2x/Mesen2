#include "com_example_mesen_demo_MesenAPI.h"
#include <Mesen/Mesen.h>
#include <jni.h>
#include <string.h>

static JavaVM* g_jvm = NULL;

static JNIEnv* get_jni_env() {
    JNIEnv* env = NULL;
    // 尝试获取JNIEnv，如果当前线程已附加则会返回JNI_OK
    jint result = g_jvm->GetEnv((void**)&env, JNI_VERSION_1_6);
    if (result == JNI_EDETACHED) {
        // 当前线程未附加，尝试附加
        if (g_jvm->AttachCurrentThread(&env, NULL) != 0) {
            // 附加失败，返回NULL或进行错误处理
            return NULL;
        }
        // 附加成功，env有效
    } else if (result == JNI_OK) {
        // 已附加，env有效
    } else {
        // 其他错误处理
        return NULL;
    }
    return env;
}



JNIEXPORT void JNICALL Java_com_example_mesen_demo_MesenAPI_init(JNIEnv*, jclass) { mesen_init(); }

JNIEXPORT void JNICALL Java_com_example_mesen_demo_MesenAPI_initializeEmu
  (JNIEnv * env, jclass, jstring home_dir, jboolean no_audio, jboolean no_video, jboolean no_input) {

    const char *c_str = env->GetStringUTFChars(home_dir, nullptr);
    mesen_initialize_emu(c_str, no_audio, no_video, no_input);
    // 使用c_str做需要的操作...

    // 用完之后，必须释放！
    env->ReleaseStringUTFChars(home_dir, c_str);


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



}

static jobject global_notification_callback = NULL;

static void notify(int event, void* param)
{
    if (event == MESEN_NOTIFICATION_TYPE_REFRESH_SOFTWARE_RENDERER) {
        auto* renderer_frame = (MesenSoftwareRendererFrame*)param;
        auto frame = renderer_frame->frame;

        JNIEnv *env = get_jni_env();

        if (!global_notification_callback ) return;

        // 1. 创建 Java int[] 数组
        int len = frame.width * frame.height;
        jintArray jbuffer = env->NewIntArray(len);
        if (!jbuffer) return;

        // 2. 拷贝 C 的数据到 Java 数组
        env->SetIntArrayRegion(jbuffer, 0, len, (const jint*)frame.buffer);

        // 3. 获取回调对象的 class
        jclass cb_class = env->GetObjectClass(global_notification_callback);

        // 4. 获取方法ID
        jmethodID mid = env->GetMethodID(
             cb_class,
            "notifyRefreshSoftwareRenderer",
            "([III)V"
        );

        if (mid) {
            // 5. 调用 Java 方法
            env->CallVoidMethod( global_notification_callback, mid, jbuffer, frame.width, frame.height);
        }

        // 6. 释放本地引用
        env->DeleteLocalRef( jbuffer);
        env->DeleteLocalRef( cb_class);
        // frame.buffer, frame.width, frame.height
    }
}


JNIEXPORT void JNICALL Java_com_example_mesen_demo_MesenAPI_registerNotificationCallback
  (JNIEnv *env, jclass, jobject callback) {

    if (global_notification_callback != NULL) {
        return;
    }

    global_notification_callback = env->NewGlobalRef( callback);
    mesen_register_notification_callback(notify);

}



// 在 JNI_OnLoad 里保存全局 JavaVM 指针
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    g_jvm = vm;
    return JNI_VERSION_1_6;
}


JNIEXPORT jboolean JNICALL Java_com_example_mesen_demo_MesenAPI_loadROM
  (JNIEnv * env, jclass, jstring file, jstring patch_file) {


    const char *file_str = env->GetStringUTFChars(file, nullptr);
    const char *patch_file_str = nullptr;

    if(patch_file != NULL) {
        patch_file_str = env->GetStringUTFChars(patch_file, nullptr);;
    }

    bool succ = mesen_load_ROM(file_str, patch_file_str);

    if(patch_file != NULL) {
        env->ReleaseStringUTFChars(patch_file, patch_file_str);
    }
    env->ReleaseStringUTFChars(file, file_str);

    return succ;

  }