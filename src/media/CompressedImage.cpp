#include <XliMedia/CompressedImage.h>

namespace Xli
{
	CompressedImage::CompressedImage(int width, int height, int depth, Format format, DataAccessor* data)
	{
		this->w = width;
		this->h = height;
		this->d = depth;
		this->format = format;
		this->data = data;
	}

	int CompressedImage::GetWidth() const
	{
		return w;
	}

	int CompressedImage::GetHeight() const
	{
		return h;
	}

	int CompressedImage::GetDepth() const
	{
		return d;
	}

	Format CompressedImage::GetFormat() const
	{
		return format;
	}

	Bitmap* CompressedImage::ToBitmap()
	{
		XLI_THROW("Unable to convert " + FormatInfo::ToString(format) + " to bitmap");
	}

	int CompressedImage::GetSizeInBytes() const
	{
		return data->GetSizeInBytes();
	}

	const UInt8* CompressedImage::GetData() const
	{
		return data->GetData();
	}
}
