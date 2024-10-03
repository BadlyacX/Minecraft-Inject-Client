#include <thread>
#include <cstdio>

#include <Windows.h>
#include <jni.h>

void MainThread(HMODULE module)
{
    JavaVM* p_jvm{ nullptr };
    jint result = JNI_GetCreatedJavaVMs(&p_jvm, 1, nullptr);

    JNIEnv* p_env{ nullptr }; // JNI interface pointer
    p_jvm->AttachCurrentThread((void**)&p_env, nullptr);

    jclass mouse_class{ p_env->FindClass("org/lwjgl/input/Mouse") };

    if (mouse_class == 0)
    {
        printf("Failed to get Mouse class\n");
        MessageBoxA(0, "ERROR", "Check console", MB_ICONERROR);
        FreeLibrary(module);
    }

    jmethodID is_button_down_id{ p_env->GetStaticMethodID(mouse_class, "isButtonDown", "(I)Z") };

    if (is_button_down_id == 0)
    {
        printf("Failed to get is_button_down id\n");
        MessageBoxA(0, "ERROR", "Check console", MB_ICONERROR);
        FreeLibrary(module);
    }

    while (!GetAsyncKeyState(VK_END))
    {
        static jint arg{ 0 };

        if (p_env->CallStaticBooleanMethodA(mouse_class, is_button_down_id, (jvalue*)&arg))
        {
            printf("Mouse clicked!\n");
        }
    }

    FreeLibrary(module);
}

bool __stdcall DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
    static FILE* p_file{ nullptr };
    static std::thread main_thread;

    if (reason == DLL_PROCESS_ATTACH)
    {
        AllocConsole();
        freopen_s(&p_file, "CONOUT$", "w", stdout);

        main_thread = std::thread([instance] { MainThread(instance); });
        if (main_thread.joinable())
            main_thread.detach();
    }
    else if (reason == DLL_PROCESS_DETACH)
    {
        fclose(p_file);
        FreeConsole();
    }

    return true;
}

/*
* Z: boolean
* B: byte
* C: char
* S: short
* I: int
* J: long
* F: float
* D: double
*/


/*
#include <windows.h>
#include <thread>
#include <cstdio>
#include <jni.h>
#include <chrono>

#include "JNI.h"
#include "Hooks.h"

std::atomic<bool> g_running{ true };

const char* MINECRAFT_OBFUSCATED_CLASS_NAME = "ave";

JavaVM* g_jvm = nullptr;

JNIEnv* GetJNIEnv() 
{
    if (!g_jvm) return nullptr;

    JNIEnv* env = nullptr;
    if (g_jvm->AttachCurrentThread(reinterpret_cast<void**>(&env), nullptr) != JNI_OK) 
    {
        printf("Failed to attach current thread to JVM\n");
        return nullptr;
    }
    return env;
}

jclass GetMinecraftClass(JNIEnv* env) 
{
    if (!env) return nullptr;

    jclass minecraftClass = env->FindClass(MINECRAFT_OBFUSCATED_CLASS_NAME);
    if (!minecraftClass) 
    {
        printf("Failed to find Minecraft class: %s\n", MINECRAFT_OBFUSCATED_CLASS_NAME);
        return nullptr;
    }
    return minecraftClass;
}

jobject GetMinecraftInstance(JNIEnv* env) 
{
    jclass minecraftClass = GetMinecraftClass(env);
    if (!minecraftClass) return nullptr;

    jmethodID getInstanceMethod = env->GetStaticMethodID(minecraftClass, "getInstance", "()Lave;");
    if (!getInstanceMethod) 
    {
        printf("Failed to find getInstance method\n");
        return nullptr;
    }

    jobject minecraftInstance = env->CallStaticObjectMethod(minecraftClass, getInstanceMethod);
    if (!minecraftInstance) 
    {
        printf("Failed to get Minecraft instance\n");
    }
    return minecraftInstance;
}

bool IsLeftMouseClicked(JNIEnv* env) 
{
    if (!env) return false;

    jclass minecraftClass = GetMinecraftClass(env);
    if (!minecraftClass) return false;
    jfieldID fieldId = env->GetFieldID(minecraftClass, "field_71415_G", "Z");
    if (!fieldId) {
        printf("Failed to find left click field\n");
        return false;
    }

    jobject minecraftInstance = GetMinecraftInstance(env);
    if (!minecraftInstance) return false;

    jboolean isLeftClicked = env->GetBooleanField(minecraftInstance, fieldId);
    return isLeftClicked == JNI_TRUE;
}

void MainThread(HMODULE instance) {
    std::unique_ptr<JNI> jni_instance = std::make_unique<JNI>();
    if (!jni_instance) 
    {
        printf("JNI initialization failed\n");
        return;
    }
    printf("JNI initialization succeeded\n");

    JNIEnv* env = jni_instance->GetEnv();
    if (!env) 
    {
        printf("Failed to get JNI environment\n");
        return;
    }
    printf("Get JNI env succeeded\n");

    while (g_running && !(GetAsyncKeyState(VK_END) & 0x8000)) 
    {
        if (IsLeftMouseClicked(env)) {
            printf("Left mouse button clicked!\n");
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    jni_instance->DetachCurrentThread();
    FreeLibraryAndExitThread(instance, 0);
}

bool __stdcall DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved) 
{
    static FILE* p_file{ nullptr };
    static std::thread main_thread;

    switch (reason)
    {
    case DLL_PROCESS_ATTACH:
        AllocConsole();
        freopen_s(&p_file, "CONOUT$", "w", stdout);
        printf("DLL Loaded\n");

        main_thread = std::thread(MainThread, instance);
        if (main_thread.joinable())
            main_thread.detach();
        break;

    case DLL_PROCESS_DETACH:
        printf("DLL Unloading\n");

        g_running = false;
        if (main_thread.joinable()) 
        {
            main_thread.join();
        }

        if (p_file) 
        {
            fclose(p_file);
            p_file = nullptr;
        }
        FreeConsole();
        break;

    default:
        break;
    }

    return true;
}
*/
