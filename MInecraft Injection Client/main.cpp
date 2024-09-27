#include <windows.h>
#include <thread>
#include <cstdio>
#include <jni.h>
#include <chrono>
#include <memory>

#include "JNI.h"
#include "Hooks.h"

std::atomic<bool> g_running{ true }; 

const char* MINECRAFT_OBFUSCATED_CLASS_NAME = "ave";

void MainThread(HMODULE instance)
{
    if (!p_jni) {
        printf("JNI initialization failed\n");
        return;
    }
    else
    {
        printf("JNI initialization succeeded\n");
    }

    JNIEnv* p_env = p_jni->GetEnv();
    if (!p_env) {
        printf("Failed to get JNI environment\n");
        return;
    }
    else
    {
        printf("Get JNI env succeeded\n");
    }

    while (g_running && !(GetAsyncKeyState(VK_END) & 0x8000)) {
        Sleep(100);  
    }

    p_jni->DetachCurrentThread();
    FreeLibraryAndExitThread(instance, 0);
}

bool __stdcall DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
    static FILE* p_file{ nullptr };
    static std::thread main_thread;

    if (reason == DLL_PROCESS_ATTACH) {
        AllocConsole();
        freopen_s(&p_file, "CONOUT$", "w", stdout);
        printf("DLL Loaded\n");
        
        p_jni = std::make_unique<JNI>();
        p_hooks = std::make_unique<Hooks>();

        main_thread = std::thread([instance] { MainThread(instance); });
        if (main_thread.joinable())
            main_thread.detach();
    }
    else if (reason == DLL_PROCESS_DETACH) {
        printf("DLL Unloading\n");

        g_running = false;

        if (main_thread.joinable()) {
            main_thread.join();
        }

        p_jni.reset();
        p_hooks.reset();

        if (p_file) {
            fclose(p_file);
            p_file = nullptr;
        }
        FreeConsole();
    }

    return true;
}

JavaVM* g_jvm = nullptr;

JNIEnv* GetJNIEnv() {
    if (!g_jvm) return nullptr;

    JNIEnv* env = nullptr;
    g_jvm->AttachCurrentThread(reinterpret_cast<void**>(&env), nullptr);
    return env;
}

jclass GetMinecraftClass() {
    JNIEnv* env = GetJNIEnv();
    if (!env) {
        printf("Failed to get JNI environment\n");
        return nullptr;
    }

    jclass minecraftClass = env->FindClass(MINECRAFT_OBFUSCATED_CLASS_NAME);
    if (!minecraftClass) {
        printf("Failed to find Minecraft class: %s \n" , MINECRAFT_OBFUSCATED_CLASS_NAME);
        return nullptr;
    }

    return minecraftClass;
}

jobject GetMinecraftInstance() {
    JNIEnv* env = GetJNIEnv();
    if (!env) {
        printf("Failed toget JNI environment \n");
        return nullptr;
    }

    jclass minecraftClass = GetMinecraftClass();
    if (!minecraftClass) {
        printf("Minecraft class not found \n");
        return nullptr;
    }

    jmethodID getInstanceMethod = env->GetStaticMethodID(minecraftClass, "getInstance", "()Lave;");
    if (!getInstanceMethod) {
        printf("Failed to find getInstance method \n");
        return nullptr;
    }

    jobject minecraftInstance = env->CallStaticObjectMethod(minecraftClass, getInstanceMethod);
    if (!minecraftInstance) {
        printf("Failed to get Minecraft instance \n");
        return nullptr;
    }

    return minecraftInstance;
}

