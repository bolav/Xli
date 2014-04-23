#ifndef __XLI_ASHIM_ANDROID_H__
#define __XLI_ASHIM_ANDROID_H__

#include <jni.h>
#include <Xli/String.h>
#include <XliHttpClient.h>

namespace Xli
{
    namespace PlatformSpecific
    {
        class AShim
        {
        private:
            static jmethodID makeNoise;
            static jmethodID raiseKeyboard;
            static jmethodID hideKeyboard;
            static jmethodID getKeyboardSize;
            static jmethodID showMessageBox;
            static jmethodID connectedToNetwork;
            static jmethodID newHttpConnection;
            static jmethodID httpGetOutputStream;
            static jmethodID httpGetInputStream;
            static jmethodID httpShowHeaders;
            static jmethodID initDefaultCookieManager;
            static jmethodID getAssetManager;
            static jmethodID hideStatusBar;
            static jmethodID showStatusBar;
            static jmethodID getStatusBarHeight;
            static jmethodID getDisplayMetrics;
            
        public:
            static int kbVisible;

            static void CacheMids(JNIEnv *env, jclass shim_class);
            static void MakeNoise();
            static void RaiseSoftKeyboard();
            static void HideSoftKeyboard();
            static bool KeyboardVisible();
            static int GetKeyboardSize();
            static void ShowStatusBar();
            static void HideStatusBar();
            static float GetStatusBarHeight();
            static int ShowMessageBox(const String& message, const String& caption, int buttons, int hints);            
            static bool ConnectedToNetwork();
            static jobject XliToJavaHeaders(const HttpRequest* req);
            static jobject SendHttpAsync(const HttpRequest* req, const void* content, long byteLength);
            static jobject SendHttpAsync(const HttpRequest* req, String content);
            static jobject SendHttpAsync(const HttpRequest* req);
            static void AbortAsyncConnection(jobject connection);
            static jobject HttpNewConnection(const String& uri, const String& method, bool hasPayload);
            static String InputStreamToString(jobject bufferedInputStream);
            static jobject AsyncInputStreamToString(jobject bufferedInputStream, HttpRequest* request);
            static jobject AsyncInputStreamToByteArray(jobject bufferedInputStream, HttpRequest* request);
            static int ReadBytesFromInputStream(jobject bufferedInputStream, int bytesToRead, void* dst);
            static AAssetManager* GetAssetManager();
            static bool RegisterNativeFunctions(JNINativeMethod native_funcs[], int funcCount);
            static void InitDefaultCookieManager();
            static float GetDensity();
            static Vector2 GetDpi();
        };
    };
};

#endif