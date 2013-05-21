#ifndef __XLI_GL_HELPER_H__
#define __XLI_GL_HELPER_H__

/**
	\defgroup GLHelper OpenGL Helper Functions
*/

#include <XliGL.h>
#include <Xli/Console.h>

namespace Xli
{
	/**
		\addtogroup GLHelper
		@{
	 */

	String GLGetErrorString(GLint err);

	static inline void GLCheckErrorImpl(const char* func, int line)
	{
		GLint err = glGetError();
		
		if (err != GL_NO_ERROR)
			ErrorPrintLine("GL ERROR: " + String::HexFromInt(err) + " - " + GLGetErrorString(err) + " (in " + func + ":" + line + ")");
	}

#define XLI_GL_CHECK_ERROR ::Xli::GLCheckErrorImpl(XLI_FUNC, XLI_LINE)

	GLuint GLCompileShader(GLenum shaderType, const String& source);
	GLuint GLLinkProgram(GLuint vsHandle, GLuint fsHandle);

	class Texture;
	Texture* GLLoadTextureData(const String& filename, Stream* stream);

	struct GLTextureInfo
	{
		GLenum Target;
		int Width;
		int Height;
		int Depth;
		int MipCount;
	};

	GLuint GLCreateTexture(Texture* texData, bool generateMips = true, GLTextureInfo* outInfo = 0);

	/** @}*/
}


#endif