#pragma once

#include <memory>
#include <Windows.h>
#include <jni.h>

class JNI final
{
public:
    JNI()
    {
        JavaVM* p_jvm{ nullptr };
        jsize vm_count{ 0 };
        jint result = JNI_GetCreatedJavaVMs(&p_jvm, 1, &vm_count);
        if (result != JNI_OK || vm_count == 0)
        {
            printf("[-] JNI() failed to initialize p_jvm\n");
            MessageBoxA(0, "ERROR", "Check console", MB_ICONASTERISK);
            return;
        }

        JavaVMAttachArgs attach_args;
        attach_args.version = JNI_VERSION_10;
        attach_args.name = nullptr;
        attach_args.group = nullptr;
        if (p_jvm->AttachCurrentThread(reinterpret_cast<void**>(&p_env), &attach_args) != JNI_OK)
        {
            printf("[-] Failed to attach current thread to JVM\n");
            MessageBoxA(0, "ERROR", "Check console", MB_ICONASTERISK);
            return;
        }

        this->p_jvm = p_jvm;
        is_init = true;
    }

    ~JNI()
    {
        if (is_init)
        {
            p_jvm->DetachCurrentThread();
            is_init = false;
        }
    }

    JNIEnv* GetEnv()
    {
        if (!is_init)
        {
            printf("[-] JNI environment not initialized.\n");
            return nullptr;
        }
        return p_env;
    }

    void DetachCurrentThread()
    {
        if (is_init && p_jvm)
        {
            p_jvm->DetachCurrentThread();
        }
    }

private:
    JavaVM* p_jvm{ nullptr };
    JNIEnv* p_env{ nullptr };
    bool is_init{ false };
};

inline std::unique_ptr<JNI> p_jni;
