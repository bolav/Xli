#ifndef __XLI_NATIVE_FILE_SYSTEM_H__
#define __XLI_NATIVE_FILE_SYSTEM_H__

#include <Xli/FileSystem.h>

namespace Xli
{
	/**
		\ingroup IO
	*/
	enum SystemDirectory
	{
		SystemDirectoryDocuments, //< Returns the path on the file system where the user keeps his personal documents. E.g. the "My Documents" folder
		SystemDirectoryRoamingAppData, //< Returns the path on the file system where to store roaming app data
		SystemDirectoryLocalAppData, //< Returns the path on the file system where to store local app data
	};

	/**
		\ingroup IO
	*/
	class NativeFileSystem: public FileSystem
	{
	public:
		/**
			Generates and returns a unique filename that can be used for creating a temporary file. The temporary file must be deleted manually by the creator.
		*/
		virtual String CreateTempFilename() = 0;

		/**
			Returns a system directory
		*/
		virtual String GetSystemDirectory(SystemDirectory dir) = 0;

		/**
			Returns the working directory
		*/
		virtual String GetCurrentDirectory() = 0;

		/**
			Sets the working directory
		*/
		virtual void ChangeDirectory(const String& dir) = 0;

		static void Init();
		static void Done();
	};

	/**
		\ingroup IO
	*/
	class DiskAccessor
	{
	public:
		NativeFileSystem* operator ->();
		operator FileSystem*();
	};

	/**
		\ingroup IO
	*/
	extern DiskAccessor Disk;

	/**
		\ingroup IO
	*/
	class AssetFileSystem
	{
	public:
		static void Init();
		static void Done();
	};

	/**
		\ingroup IO
	*/
	class AssetsAccessor
	{
	public:
		void SetFilesystem(FileSystem* fs);
		FileSystem* operator ->();
		operator FileSystem*();
	};

	/**
		\ingroup IO
	*/
	extern AssetsAccessor Assets;
}


#endif
