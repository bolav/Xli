#ifndef __XLI_HTTP_URI_H__
#define __XLI_HTTP_URI_H__

#include <Xli/String.h>

namespace Xli
{
	class Uri
	{
	public:
		static String Encode(const String& uri);
		static String Decode(const String& uri);
	};
}

#endif
