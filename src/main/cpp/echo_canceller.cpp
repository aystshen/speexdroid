#include <jni.h>
#include <android/log.h>
#include "speex/speex_echo.h"
#include "speex/speex_preprocess.h"

#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, "speexdroid", __VA_ARGS__))
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "speexdroid", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "speexdroid", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "speexdroid", __VA_ARGS__))

SpeexEchoState *st;
SpeexPreprocessState *den;

/**
 * 初始化算法模块
 * @param env
 * @param obj
 * @param jSampleRate 采样率
 * @param jFrameSize 一次处理的样品数量(应对应于10-20 ms)
 * @param jFilterLength 要消回的样本数(一般应对应于100-500毫秒)
 */
void open(JNIEnv *env, jobject obj, jint jSampleRate, jint jFrameSize, jint jFilterLength) {
    LOGI("%s, sampleRate=%d, frameSize=%d, filterLength=%d\n", __func__, jSampleRate, jFrameSize,
         jFilterLength);

    int sampleRate = jSampleRate;
    st = speex_echo_state_init(jFrameSize, jFilterLength);
    den = speex_preprocess_state_init(jFrameSize, sampleRate);
    speex_echo_ctl(st, SPEEX_ECHO_SET_SAMPLING_RATE, &sampleRate);
    speex_preprocess_ctl(den, SPEEX_PREPROCESS_SET_ECHO_STATE, st);
}

/**
 * 执行一帧回音消除
 * @param env
 * @param obj
 * @param jRecFrame 麦克风信号
 * @param jRefFrame 参考信号
 * @return 消回信号
 */
jshortArray process(JNIEnv *env, jobject obj, jshortArray jRecFrame, jshortArray jRefFrame) {
    LOGI("%s", __func__);

    //create native shorts from java shorts
    jshort *recFrame = env->GetShortArrayElements(jRecFrame, 0);
    jshort *refFrame = env->GetShortArrayElements(jRefFrame, 0);

    //allocate memory for output data
    jint length = env->GetArrayLength(jRecFrame);
    jshortArray temp = env->NewShortArray(length);
    jshort *outFrame = env->GetShortArrayElements(temp, 0);

    //call echo cancellation
    speex_echo_cancellation(st, recFrame, refFrame, outFrame);
    //preprocess output frame
    speex_preprocess_run(den, outFrame);

    //convert native output to java layer output
    jshortArray output = env->NewShortArray(length);
    env->SetShortArrayRegion(output, 0, length, outFrame);

    //cleanup and return
    env->ReleaseShortArrayElements(jRecFrame, recFrame, 0);
    env->ReleaseShortArrayElements(jRefFrame, refFrame, 0);
    env->ReleaseShortArrayElements(temp, outFrame, 0);

    return output;
}

/**
 * 销毁算法模块
 * @param env
 * @param obj
 */
void close(JNIEnv *env, jobject obj) {
    LOGI("%s", __func__);

    speex_echo_state_destroy(st);
    speex_preprocess_state_destroy(den);
    st = nullptr;
    den = nullptr;
}

JNINativeMethod methods[] = {
        {"open",    "(III)V",   (void *) open},
        {"process", "([S[S)[S", (void *) process},
        {"close",   "()V",      (void *) close},
};

jint register_native_method(JNIEnv *env) {
    jclass cl = env->FindClass("com/speex/EchoCanceller");
    if ((env->RegisterNatives(cl, methods, sizeof(methods) / sizeof(methods[0]))) < 0) {
        return -1;
    }
    return 0;
}

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = nullptr;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }
    if (register_native_method(env) != JNI_OK) {
        return -1;
    }
    return JNI_VERSION_1_6;
}