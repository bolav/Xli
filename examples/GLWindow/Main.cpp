#include <Xli.h>
#include <XliGL.h>
#include <XliPlatform.h>

using namespace Xli;

class GLApp: public Application
{
    Managed<GLContext> _gl;
    bool _background;
    
    int _fpsCount;
    double _fpsTime;
    double _lastTime;

    int _dialogType;
    double _touchDownTime;
    double _tapTime;

public:
    GLApp()
    {
        PrintLine("Constructor");

        _dialogType = 0;
        _touchDownTime = 0;
        _tapTime = 0;

        _fpsCount = 0;
        _fpsTime = 0;
        _lastTime = 0;

        _background = false;
    }

    virtual ~GLApp()
    {
        PrintLine("Destructor");
    }

    virtual void OnInit(Window* wnd)
    {
        PrintLine("OnInit");
        
        // Setup OpenGL

        GLContextAttributes glAttribs = GLContextAttributes::Default();
        glAttribs.Samples = 16;

        _gl = GLContext::Create(wnd, glAttribs);
        _gl->GetAttributes(glAttribs);
        _gl->SetSwapInterval(0);
        
        glClearColor(1, 0, 0, 1);

        // Print platform info

		PrintLine((String)"Time: " + DateTime::Now().ToString());
		PrintLine((String)"Time (UTC): " + DateTime::NowUtc().ToString());

		PrintLine((String)"Window Client Size: " + wnd->GetClientSize().ToString());

		PrintLine((String)"OpenGL Vendor: " + (const char*)glGetString(GL_VENDOR));
		PrintLine((String)"OpenGL Renderer: " + (const char*)glGetString(GL_RENDERER));
		PrintLine((String)"OpenGL Version: " + (const char*)glGetString(GL_VERSION));
		PrintLine((String)"OpenGL Drawable Size: " + _gl->GetDrawableSize().ToString());
		PrintLine((String)"OpenGL Swap Interval: " + _gl->GetSwapInterval());
		PrintLine((String)"OpenGL Color Bits: " + glAttribs.ColorBits.ToString());
		PrintLine((String)"OpenGL Depth Bits: " + glAttribs.DepthBits);
		PrintLine((String)"OpenGL Stencil Bits: " + glAttribs.StencilBits);
		PrintLine((String)"OpenGL Accum Bits: " + glAttribs.AccumBits.ToString());
		PrintLine((String)"OpenGL Buffers: " + glAttribs.Buffers);
		PrintLine((String)"OpenGL Stereo: " + String::FromBool(glAttribs.Stereo));

		PrintLine((String)"FileSystem Base Dir: " + Disk->GetBaseDirectory());
		PrintLine((String)"FileSystem Working Dir: " + Disk->GetCurrentDirectory());
		PrintLine((String)"FileSystem Config Dir: " + Disk->GetSystemDirectory(SystemDirectoryConfig));
		PrintLine((String)"FileSystem Data Dir: " + Disk->GetSystemDirectory(SystemDirectoryData));
		PrintLine((String)"FileSystem Desktop Dir: " + Disk->GetSystemDirectory(SystemDirectoryDesktop));
		PrintLine((String)"FileSystem Downloads Dir: " + Disk->GetSystemDirectory(SystemDirectoryDownloads));
		PrintLine((String)"FileSystem Templates Dir: " + Disk->GetSystemDirectory(SystemDirectoryTemplates));
		PrintLine((String)"FileSystem Public Dir: " + Disk->GetSystemDirectory(SystemDirectoryPublic));
		PrintLine((String)"FileSystem Documents Dir: " + Disk->GetSystemDirectory(SystemDirectoryDocuments));
		PrintLine((String)"FileSystem Music Dir: " + Disk->GetSystemDirectory(SystemDirectoryMusic));
		PrintLine((String)"FileSystem Pictures Dir: " + Disk->GetSystemDirectory(SystemDirectoryPictures));
		PrintLine((String)"FileSystem Videos Dir: " + Disk->GetSystemDirectory(SystemDirectoryVideos));
		PrintLine((String)"FileSystem Temp Filename: " + Disk->CreateTempFilename());

		PrintLine((String)"DPI: " + Display::GetDpi(0).ToString());
		PrintLine((String)"statusbar size: " + wnd->GetStatusBarSize().ToString());
    }

    virtual void OnLoad(Window* wnd)
    {
        PrintLine("OnLoad");

        _lastTime = GetSeconds();
    }

    virtual void OnDraw(Window* wnd)
    {
        double t = GetSeconds();
        _fpsTime += (t - _lastTime);
        _lastTime = t;
        _fpsCount++;

        if (_fpsTime >= 1.0)
        {
            PrintLine("OnDraw, FPS: " + String::Format("%.2f", (double)_fpsCount / _fpsTime) + ", Background: " + String::FromBool(_background));
            _fpsCount = 0;
            _fpsTime = 0;
        }

    	if (_background)
    	{
    		Sleep(100);
    		return;
    	}

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // TODO: Draw something
        
        _gl->SwapBuffers();

        if (wnd->GetKeyState(KeySpace))
        {
            Vector2i mousePos = wnd->GetMousePosition();
            wnd->SetTitle(String::Format("%.2lf %d %d", GetSeconds(), mousePos.X, mousePos.Y));
        }
    }

    virtual bool OnKeyDown(Window* wnd, Key key)
    {
        PrintLine("OnKeyDown: " + (String)(int)key);

        if (key == KeyF11 || (key == KeyEnter && wnd->GetKeyState(KeyCtrl)))
        {
            wnd->SetFullscreen(!wnd->IsFullscreen());
            return true;
        }
        else if (key == KeyF4 || key == KeyAlt)
        {
            // Set as handled to disable default closing behaviour on Win32 (TODO: This does not work as expected)
            return true;
        }

        return false;
    }

    virtual bool OnKeyUp(Window* wnd, Key key)
    {
        PrintLine("OnKeyUp: " + (String)(int)key);
        return false;
    }

    virtual bool OnTextInput(Window* wnd, const String& text)
    {
        PrintLine("OnTextInput: " + text);
        return false;
    }

    virtual bool OnMouseDown(Window* wnd, Vector2i pos, MouseButton button)
    {
        PrintLine("OnMouseDown: " + pos.ToString() + ", " + (String)(int)button);
        return false;
    }

    virtual bool OnMouseUp(Window* wnd, Vector2i pos, MouseButton button)
    {
        PrintLine("OnMouseUp: " + pos.ToString() + ", " + (String)(int)button);
        return false;
    }

    virtual bool OnMouseMove(Window* wnd, Vector2i pos)
    {
        PrintLine("OnMouseMove: " + pos.ToString());

        if (wnd->GetMouseButtonState(MouseButtonLeft))
        {
            Vector2i size = wnd->GetClientSize();

            float x = (float)pos.X / size.X;
            float y = (float)pos.Y / size.Y;

            glClearColor(x, y, 0, 1);
            return true;
        }
        else if (wnd->GetMouseButtonState(MouseButtonMiddle))
        {
            Vector2i size = wnd->GetClientSize();

            float x = (float)pos.X / size.X;
            float y = (float)pos.Y / size.Y;

            glClearColor(x, y, x, 1);
            return true;
        }
        else if (wnd->GetMouseButtonState(MouseButtonRight))
        {
            Vector2i size = wnd->GetClientSize();

            float x = (float)pos.X / size.X;
            float y = (float)pos.Y / size.Y;

            glClearColor(y, 0, x, 1);
            return true;
        }

        return false;
    }

    virtual bool OnMouseWheel(Window* wnd, Vector2i delta)
    {
        PrintLine("OnMouseWheel: " + delta.ToString());
        return false;
    }

    virtual bool OnTouchDown(Window* wnd, Vector2 pos, int id)
    {
        PrintLine("OnTouchDown: " + pos.ToString() + ", " + id);
        _touchDownTime = GetSeconds();
        return false;
    }

    virtual bool OnTouchMove(Window* wnd, Vector2 pos, int id)
    {
        PrintLine("OnTouchMove: " + pos.ToString() + ", " + id);
        Vector2i size = wnd->GetClientSize();
        glClearColor(pos.X / size.X, pos.Y / size.Y, 0, 1);
        return false;
    }

    virtual bool OnTouchUp(Window* wnd, Vector2 pos, int id)
    {
        PrintLine("OnTouchUp: " + pos.ToString() + ", " + id);
        double currentTime = GetSeconds();

        if (currentTime - _touchDownTime < 0.15)
        {
            if (currentTime - _tapTime < 0.3)
            {
                PrintLine("Bang");
                wnd->BeginTextInput((Xli::TextInputHint)0);
            }
            else if (wnd->IsTextInputActive())
            {
                wnd->EndTextInput();
            }

            _tapTime = currentTime;
        }
        
        return false;
    }

    virtual void OnNativeHandleChanged(Window* wnd)
    {
        PrintLine("OnNativeHandleChanged");
        _gl->MakeCurrent(wnd);
    }

    virtual void OnSizeChanged(Window* wnd)
    {
        Vector2i clientSize = wnd->GetClientSize();
        PrintLine("OnSizeChanged: " + clientSize.ToString());
        glViewport(0, 0, clientSize.X, clientSize.Y);
        Application::OnSizeChanged(wnd);
    }

    virtual bool OnClosing(Window* wnd)
    {
        PrintLine("OnClosing");
        return MessageBox::Show(wnd, "Close?", "Close?", DialogButtonsYesNo, DialogHintQuestion | DialogHintButton2Default) == DialogResultNo;
    }

    virtual void OnClosed(Window* wnd)
    {
        PrintLine("OnClosed");
    }

    virtual void OnAppLowMemory(Window* wnd)
    {
        PrintLine("OnAppLowMemory");
    }

    virtual void OnAppTerminating(Window* wnd)
    {
        PrintLine("OnAppTerminating");
    }

    virtual void OnAppWillEnterForeground(Window* wnd)
    {
        PrintLine("OnAppWillEnterForeground");
    }

    virtual void OnAppDidEnterForeground(Window* wnd)
    {
        PrintLine("OnAppDidEnterForeground");
        _background = false;
    }

    virtual void OnAppWillEnterBackground(Window* wnd)
    {
        PrintLine("OnAppWillEnterBackground");
    }

    virtual void OnAppDidEnterBackground(Window* wnd)
    {
        PrintLine("OnAppDidEnterBackground");
        _background = true;
    }
};

int Main(const Array<String>& args)
{
    GLApp app;
    Application::Run(&app);
    return 0;
}
