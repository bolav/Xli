#ifndef __XLI_PLATFORM_DISPLAY_H__
#define __XLI_PLATFORM_DISPLAY_H__

#include <Xli/Rectangle.h>

namespace Xli
{
	struct DisplaySettings
	{
		Vector2i Resolution;
		int RefreshRate, BitsPerPixel;
	};

	class Display
	{
	public:
		static void Init();
		static void Shutdown();

		/**
			Returns the number of screens connected to the system.
		*/
		static int GetCount();

		/**
			Returns the coordinate system of the specified screen.
		*/
		static Recti GetRect(int index);

		// TODO: Interfaces to be implemented later
		//static void GetCurrentSettings(int index, DisplaySettings& settings);
		//static void GetSupportedSettings(int index, Array<DisplaySettings>& settings);
		//static void ChangeSettings(int index, const DisplaySettings& settings);
	};
}


#endif
