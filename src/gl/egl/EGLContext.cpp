#include <XliGL.h>
#include <Xli/Shared.h>
#include <EGL/egl.h>
#include <stdlib.h>

#ifdef XLI_PLATFORM_ANDROID
# include <android/native_window.h>
# define NATIVE_HANDLE ANativeWindow*
#endif

namespace Xli
{
    class EglContext: public GLContext
    {
        Shared<Window> window;
        EGLDisplay display;
        EGLSurface surface;
        EGLContext context;
        EGLConfig config;
        int swapInterval;

    public:
        EglContext(Window* wnd, const GLContextAttributes& attribs)
        {
            context = EGL_NO_CONTEXT;
            surface = EGL_NO_SURFACE;

            display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
            eglInitialize(display, 0, 0);

            const EGLint iattribs[] =
            {
                EGL_RED_SIZE, 5,
                EGL_GREEN_SIZE, 6,
                EGL_BLUE_SIZE, 5,
                EGL_ALPHA_SIZE, 0,
                EGL_DEPTH_SIZE, 16,
                EGL_STENCIL_SIZE, 0,
                EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                //EGL_RENDER_BUFFER, attribs.Buffers <= 1 ? EGL_SINGLE_BUFFER : EGL_BACK_BUFFER,
                EGL_NONE
            };

            EGLint numConfigs;
            EGLConfig configs[128];

            if (!eglChooseConfig(display, iattribs, configs, 128, &numConfigs) || numConfigs == 0)
                XLI_THROW("Unable to choose EGL config");
        
            EGLint cs = 0, cd = 0, cb = 0;
            int cc = 0;

            for (int i = 0; i < numConfigs; i++)
            {
                EGLint samples, depth, stencil, buffer, r, g, b, a, render;
                eglGetConfigAttrib(display, configs[i], EGL_RED_SIZE, &r);
                eglGetConfigAttrib(display, configs[i], EGL_GREEN_SIZE, &g);
                eglGetConfigAttrib(display, configs[i], EGL_BLUE_SIZE, &b);
                eglGetConfigAttrib(display, configs[i], EGL_ALPHA_SIZE, &a);
                eglGetConfigAttrib(display, configs[i], EGL_BUFFER_SIZE, &buffer);
                eglGetConfigAttrib(display, configs[i], EGL_DEPTH_SIZE, &depth);
                eglGetConfigAttrib(display, configs[i], EGL_STENCIL_SIZE, &stencil);
                eglGetConfigAttrib(display, configs[i], EGL_SAMPLES, &samples);
                eglGetConfigAttrib(display, configs[i], EGL_RENDER_BUFFER, &render);

#ifdef XLI_DEBUG
                ErrorPrintLine(String::Format("EGL Config %d:  M %d  D %d  S %d  B %d  R %d  G %d  B %d  A %d  Single %d", i, samples, depth, stencil, buffer, r, g, b, a, render == EGL_SINGLE_BUFFER));
#endif

                if (samples >= cs && depth >= cd && buffer >= cb && 
                    samples <= attribs.Samples && r <= attribs.ColorBits.R && g <= attribs.ColorBits.G && b <= attribs.ColorBits.B && a <= attribs.ColorBits.A)
                {
                    cs = samples;
                    cd = depth;
                    cb = buffer;
                    cc = i;
                }
            }

#ifdef XLI_DEBUG
            ErrorPrintLine((String)"Selected EGL config: " + (int)cc);
#endif

            config = configs[cc];
            swapInterval = -1;

            SetWindow(wnd);
            
        }

        virtual ~EglContext()
        {
            if (display != EGL_NO_DISPLAY)
            {
                eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

                if (context != EGL_NO_CONTEXT)
                    eglDestroyContext(display, context);

                if (surface != EGL_NO_SURFACE)
                    eglDestroySurface(display, surface);

                eglTerminate(display);
            }
        }

        virtual void SetWindow(Window* window)
        {
            this->window = window;

#ifdef XLI_PLATFORM_ANDROID
            if (window->GetImplementation() == WindowImplementationAndroid)
            {
                EGLint format;
                eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
                ANativeWindow_setBuffersGeometry((ANativeWindow*)window->GetNativeHandle(), 0, 0, format);
            }
#endif

            if (surface != EGL_NO_SURFACE) 
                eglDestroySurface(display, surface);
            
            surface = eglCreateWindowSurface(display, config, (NATIVE_HANDLE)window->GetNativeHandle(), NULL);

            if (surface == EGL_NO_SURFACE)
                XLI_THROW("Unable to create EGL Surface");

            if (context == EGL_NO_CONTEXT)
            {
                const EGLint context_attribs[] =
                {
                    EGL_CONTEXT_CLIENT_VERSION, 2,
                    EGL_NONE
                };

                context = eglCreateContext(display, config, EGL_NO_CONTEXT, context_attribs);

                if (context == EGL_NO_CONTEXT)
                    XLI_THROW("Unable to create EGL Context");
            }
    
            MakeCurrent(true);
        }

        virtual Window* GetWindow()
        {
            return window;
        }

        virtual GLContext* CreateSharedContext()
        {
            XLI_THROW_NOT_SUPPORTED(XLI_FUNCTION);
        }

        virtual void MakeCurrent(bool current)
        {
            if (eglMakeCurrent(display, surface, surface, current ? context : 0) == EGL_FALSE)
                XLI_THROW("Unable to make EGL context current");
        }

        virtual bool IsCurrent()
        {
            return eglGetCurrentContext() == context;
        }

        virtual void SwapBuffers()
        {
            eglSwapBuffers(display, surface);
        }
        
        virtual void SetSwapInterval(int value)
        {
            if (eglSwapInterval(display, value))
                swapInterval = value;
        }

        virtual int GetSwapInterval()
        {
            return swapInterval;
        }

        virtual Vector2i GetBackbufferSize()
        {
            return window->GetClientSize();
        }

        virtual void GetAttributes(GLContextAttributes& result)
        {
            memset(&result, 0, sizeof(GLContextAttributes));
            eglGetConfigAttrib(display, config, EGL_RED_SIZE, &result.ColorBits.R);
            eglGetConfigAttrib(display, config, EGL_GREEN_SIZE, &result.ColorBits.G);
            eglGetConfigAttrib(display, config, EGL_BLUE_SIZE, &result.ColorBits.B);
            eglGetConfigAttrib(display, config, EGL_ALPHA_SIZE, &result.ColorBits.A);
            eglGetConfigAttrib(display, config, EGL_DEPTH_SIZE, &result.DepthBits);
            eglGetConfigAttrib(display, config, EGL_STENCIL_SIZE, &result.StencilBits);
            eglGetConfigAttrib(display, config, EGL_SAMPLES, &result.Samples);

            EGLint renderBuffer;
            eglGetConfigAttrib(display, config, EGL_RENDER_BUFFER, &renderBuffer);
            result.Buffers = renderBuffer == EGL_SINGLE_BUFFER ? 1 : 2;
        }
    };

    GLContext* GLContext::Create(Window* window, const GLContextAttributes& attribs)
    {
        return new EglContext(window, attribs);
    }
}
