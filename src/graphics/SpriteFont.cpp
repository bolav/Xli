#include <XliGraphics/SpriteFont.h>

namespace Xli
{
	SpriteFont::SpriteFont(BitmapFont* bf)
	{
		if (!bf) XLI_THROW_NULL_POINTER;

		Ascender = bf->Ascender;
		Descender = bf->Descender;
		SpaceWidth = bf->SpaceWidth;

		float winv = 1.0f / (float)bf->Bitmap->GetWidth();
		float hinv = 1.0f / (float)bf->Bitmap->GetHeight();

		for (int it = bf->Characters.Begin(); it != bf->Characters.End(); it = bf->Characters.Next(it))
		{
			Utf32Char c = bf->Characters.GetKey(it);
			const BitmapFont::CharacterData& bfcd = bf->Characters.GetValue(it);

			Vector2 ll, ul, ur, lr;
			ll.X = (float)bfcd.LowerLeft.X * winv;
			ll.Y = (float)bfcd.LowerLeft.Y * hinv;
			ul.X = (float)bfcd.UpperLeft.X * winv;
			ul.Y = (float)bfcd.UpperLeft.Y * hinv;
			ur.X = (float)bfcd.UpperRight.X * winv;
			ur.Y = (float)bfcd.UpperRight.Y * hinv;
			lr.X = (float)bfcd.LowerRight.X * winv;
			lr.Y = (float)bfcd.LowerRight.Y * hinv;

			CharacterData cd;
			cd.LowerLeft.X = (UInt16)Clamp((int)((float)0xffff * ll.X), 0, 0xffff);
			cd.LowerLeft.Y = (UInt16)Clamp((int)((float)0xffff * ll.Y), 0, 0xffff);
			cd.UpperLeft.X = (UInt16)Clamp((int)((float)0xffff * ul.X), 0, 0xffff);
			cd.UpperLeft.Y = (UInt16)Clamp((int)((float)0xffff * ul.Y), 0, 0xffff);
			cd.UpperRight.X = (UInt16)Clamp((int)((float)0xffff * ur.X), 0, 0xffff);
			cd.UpperRight.Y = (UInt16)Clamp((int)((float)0xffff * ur.Y), 0, 0xffff);
			cd.LowerRight.X = (UInt16)Clamp((int)((float)0xffff * lr.X), 0, 0xffff);
			cd.LowerRight.Y = (UInt16)Clamp((int)((float)0xffff * lr.Y), 0, 0xffff);
			cd.Advance = bfcd.Advance;
			cd.Bearing = bfcd.Bearing;
			cd.Size = bfcd.Size;
			cd.Kerning = bfcd.Kerning;

			Characters.Add(c, cd);
		}
	}

	void SpriteFont::WriteCharacter(const CharacterData& cd, const Vector2i& cursor, Array<Vertex>& output) const
	{
		Vertex a, b, c, d;
		a.Position.X = (Int16)(cursor.X + cd.Bearing.X);
		a.Position.Y = (Int16)(cursor.Y + cd.Bearing.Y + cd.Size.Y);
		a.TexCoord = cd.LowerLeft;
		b.Position.X = (Int16)(cursor.X + cd.Bearing.X + cd.Size.X);
		b.Position.Y = (Int16)(cursor.Y + cd.Bearing.Y + cd.Size.Y);
		b.TexCoord = cd.LowerRight;
		c.Position.X = (Int16)(cursor.X + cd.Bearing.X + cd.Size.X);
		c.Position.Y = (Int16)(cursor.Y + cd.Bearing.Y);
		c.TexCoord = cd.UpperRight;
		d.Position.X = (Int16)(cursor.X + cd.Bearing.X);
		d.Position.Y = (Int16)(cursor.Y + cd.Bearing.Y);
		d.TexCoord = cd.UpperLeft;

		output.Add(a);
		output.Add(b);
		output.Add(c);
		output.Add(d);
	}

	int SpriteFont::GetStringWidth(const String& str) const
	{
		int len = 0;
		Utf32Char prev = -1;

		for (int i = 0; i < str.Length(); i++)
		{
			Utf32Char c = str[i];

			CharacterData cd;
			if ((c < 0xff && isspace(c)) || !Characters.Get(c, cd))
			{
				len += SpaceWidth;
				prev = c;
				continue;
			}

			int k = 0;
			cd.Kerning.Get(prev, k);

			len += cd.Advance + k;
			prev = c;
		}

		return len;
	}

	int SpriteFont::GetLineHeight() const
	{
		return Ascender + Descender;
	}

	Vector2i SpriteFont::WriteString(const String& str, Vector2i cursor, const Recti& cullRect, Array<Vertex>& output) const
	{
		if (cursor.Y + Descender <= cullRect.Top || cursor.Y - Ascender > cullRect.Bottom)
		{
			cursor.X += GetStringWidth(str);
			return cursor;
		}

		Utf32Char prev = -1;

		for (int i = 0; i < str.Length(); i++)
		{
			Utf32Char c = str[i];

			CharacterData cd;
			if ((c < 0xff && isspace(c)) || !Characters.Get(c, cd))
			{
				cursor.X += SpaceWidth;
				continue;
			}

			int k = 0;
			cd.Kerning.Get(prev, k);
			cursor.X += k;

			if (cursor.X + cd.Advance >= cullRect.Left && cursor.X <= cullRect.Right)
			{
				WriteCharacter(cd, cursor, output);
			}

			cursor.X += cd.Advance;
			prev = c;
		}

		return cursor;
	}

	void SpriteFont::WriteString(const String& str, const Recti& rect, int textAlign, bool wordWrap, Array<Vertex>& output) const
	{
		Array<String> lines;
		
		if (wordWrap) WordWrap(str, rect.Size(), lines);
		else str.Split('\n', lines);

		if (!lines.Length()) return;

		Vector2i cursor = rect.Position();

		if (textAlign & TextAlignBottom)
		{
			int height = lines.Length() * (Ascender + Descender);
			cursor.Y += rect.Height() - height;
		}
		else if (textAlign & TextAlignMiddle)
		{
			int height = lines.Length() * (Ascender + Descender);
			cursor.Y += (rect.Height() - height + Descender) / 2;
		}

		for (int i = 0; i < lines.Length(); i++)
		{
			cursor.Y += Ascender;
			cursor.X = rect.Left;

			if (textAlign & TextAlignRight)
			{
				int width = GetStringWidth(lines[i]);
				cursor.X += rect.Width() - width;
			}
			if (textAlign & TextAlignCenter)
			{
				int width = GetStringWidth(lines[i]);
				cursor.X += (rect.Width() - width) / 2;
			}

			WriteString(str, cursor, rect, output);

			cursor.Y += Descender;
		}
	}

	void SpriteFont::WordWrap(const String& str, Vector2i size, Array<String>& result, int startPosition, int* endPosition, int startIndex, int* endIndex) const
	{
		if (size.Y <= 0 || size.X <= 0)
		{
			if (endPosition) *endPosition = startPosition;
			if (endIndex) *endIndex = startIndex;
			return;
		}

		Vector2i cursor(startPosition, 0);

		int lineStart = startIndex, wordStart = startIndex, spaceStart = startIndex;
		Utf32Char prev = 0;
		bool word = false;

		if (endPosition) *endPosition = 0;

		for (int i = startIndex; i < str.Length(); i++)
		{
			Utf32Char c = str[i];

			CharacterData cd;
			if (c < 0xff && isspace(c))
			{
				if (word)
				{
					spaceStart = i;
					word = false;
				}

				cursor.X += SpaceWidth;
				prev = c;

				if (c == '\n')
				{
					result.Add(str.SubString(lineStart, i - spaceStart));
					cursor.Y += Ascender + Descender;
					if (cursor.Y >= size.Y)
					{
						if (endIndex) *endIndex = i + 1;
						return;
					}
					cursor.X = 0;
					lineStart = i + 1;
					spaceStart = i + 1;
					prev = 0;
				}

				continue;
			}

			if (!word)
			{
				wordStart = i;
				word = true;
			}

			if (Characters.Get(c, cd))
			{
				int k = 0;
				cd.Kerning.Get(prev, k);
				cursor.X += cd.Advance + k;
			}
			else
			{
				cursor.X += SpaceWidth;
			}

			if (cursor.X > size.X)
			{
				if (lineStart == wordStart && result.Length() == 0 && startPosition > 0) // Special case for multifont word wrapping
				{
					result.Add("");
					cursor.Y += Ascender + Descender;
					if (cursor.Y >= size.Y)
					{
						if (endIndex) *endIndex = wordStart;
						return;
					}
					cursor.X = 0;
					i = lineStart - 1;
				}
				else if (lineStart < wordStart)
				{
					result.Add(str.SubString(lineStart, spaceStart - lineStart));
					cursor.Y += Ascender + Descender;
					if (cursor.Y >= size.Y)
					{
						if (endIndex) *endIndex = wordStart;
						return;
					}
					cursor.X = 0;
					lineStart = wordStart;
					i = wordStart - 1;
				}
				else
				{
					result.Add(str.SubString(lineStart, i - lineStart));
					cursor.Y += Ascender + Descender;
					if (cursor.Y >= size.Y)
					{
						if (endIndex) *endIndex = i;
						return;
					}
					cursor.X = 0;
					lineStart = i;
					i = i - 1;
				}

				prev = 0;
			}
			else
			{
				prev = c;
			}
		}

		if (lineStart < str.Length())
		{
			String lastLine = str.SubString(lineStart, str.Length() - lineStart);
			result.Add(lastLine);
			if (endPosition) *endPosition = GetStringWidth(lastLine);
		}

		if (endIndex) *endIndex = str.Length();
	}
}
