#include <XliPlatform/MessageBox.h>
#include <XliPlatform/Window.h>
#include <XliPlatform/Thread.h>
#include <Xli/StringBuilder.h>

namespace Xli
{
	void MessageBox::HandleException(const Exception& e, const String& catcher, Window* wnd)
	{
		StringBuilder sb(2048);

		sb.Append(e.GetMessage());

		sb.Append("\n\nFunction: ");
		sb.Append(e.GetFunction());
		sb.Append("\nLine: ");
		sb.Append(e.GetLine());

		if (catcher.Length())
		{
			sb.Append("\n\nCaught by: ");
			sb.Append(catcher);
		}

		if (!wnd)
		{
			wnd = Window::GetMainWindow();
		}

		Show(wnd, sb, "Unhandled exception", Xli::DialogButtonsOK, Xli::DialogHintError);
	}
}
