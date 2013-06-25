#ifndef __XLI_VECTOR4_H__
#define __XLI_VECTOR4_H__

#include "Vector3.h"

namespace Xli
{
	/**
		\ingroup Math
	*/
	template <typename T> class Vector4t
	{
	public:
		union
		{
			struct { T X, Y, Z, W; };
			struct { T R, G, B, A; };
			T Comps[4];
		}; 

		T* Data()
		{
			return Comps;
		}

		const T* Data() const
		{
			return Comps;
		}

		operator T* ()
		{
			return Comps;
		}

		operator const T* () const
		{
			return Comps;
		}

		T& operator [] (int i)
		{
#ifdef XLI_RANGE_CHECK
			if (i >= 4 || i < 0)
				XLI_THROW_INDEX_OUT_OF_BOUNDS;
#endif
			return Comps[i];
		}

		String ToString() const
		{
			return String(X) + ", " + Y + ", " + Z + ", " + W;
		}

		Vector2t<T> XY() const
		{
			return Vector2t<T>(X, Y);
		}

		Vector3t<T> XYZ() const
		{
			return Vector3t<T>(X, Y, Z);
		}

		Vector4t(const Vector2t<T>& xy, T z, T w)
		{
			X = xy.X;
			Y = xy.Y;
			Z = z;
			W = w;
		}

		Vector4t(const Vector3t<T>& xyz, T w)
		{
			X = xyz.X;
			Y = xyz.Y;
			Z = xyz.Z;
			W = w;
		}

		Vector4t()
		{
		}

		Vector4t(const T& x, const T& y, const T& z, const T& w)
		{
			X = x;
			Y = y;
			Z = z;
			W = w;
		}

		Vector4t(const Vector4t& v)
		{
			X = v.X;
			Y = v.Y;
			Z = v.Z;
			W = v.W;
		}

		template <typename U> explicit Vector4t(const Vector4t<U>& v)
		{
			X = (T)v.X;
			Y = (T)v.Y;
			Z = (T)v.Z;
			W = (T)v.W;
		}

		Vector4t& operator = (const Vector4t& v)
		{
			X = v.X;
			Y = v.Y;
			Z = v.Z;
			W = v.W;
			return *this;
		}

		bool operator == (const Vector4t& v) const
		{
			return (X == v.X) && (Y == v.Y) && (Z == v.Z) && (W == v.W);
		}

		bool operator != (const Vector4t& v) const
		{
			return !(*this == v);
		}

		Vector4t operator - () const
		{
			Vector4t r;
			r.X = -X;
			r.Y = -Y;
			r.Z = -Z;
			r.W = -W;
			return r;
		}

		Vector4t operator + (const Vector4t& v) const
		{
			Vector4t r;
			r.X = X + v.X;
			r.Y = Y + v.Y;
			r.Z = Z + v.Z;
			r.W = W + v.W;
			return r;
		}

		Vector4t operator - (const Vector4t& v) const
		{
			Vector4t r;
			r.X = X - v.X;
			r.Y = Y - v.Y;
			r.Z = Z - v.Z;
			r.W = W - v.W;
			return r;
		}
		Vector4t operator * (const Vector4t& v) const
		{
			Vector4t r;
			r.X = X * v.X;
			r.Y = Y * v.Y;
			r.Z = Z * v.Z;
			r.W = W * v.W;
			return r;
		}

		Vector4t operator / (const Vector4t& v) const
		{
			Vector4t r;
			r.X = X / v.X;
			r.Y = Y / v.Y;
			r.Z = Z / v.Z;
			r.W = W / v.W;
			return r;
		}

		Vector4t operator + (const T& s) const
		{
			Vector4t r;
			r.X = X + s;
			r.Y = Y + s;
			r.Z = Z + s;
			r.W = W + s;
			return r;
		}

		Vector4t operator - (const T& s) const
		{
			Vector4t r;
			r.X = X - s;
			r.Y = Y - s;
			r.Z = Z - s;
			r.W = W - s;
			return r;
		}

		Vector4t operator * (const T& s) const
		{
			Vector4t r;
			r.X = X * s;
			r.Y = Y * s;
			r.Z = Z * s;
			r.W = W * s;
			return r;
		}

		Vector4t operator / (const T& s) const
		{
			Vector4t r;
			r.X = X / s;
			r.Y = Y / s;
			r.Z = Z / s;
			r.W = W / s;
			return r;
		}

		Vector4t& operator += (const Vector4t& v)
		{
			X += v.X;
			Y += v.Y;
			Z += v.Z;
			W += v.W;
			return *this;
		}

		Vector4t& operator -= (const Vector4t& v)
		{
			X -= v.X;
			Y -= v.Y;
			Z -= v.Z;
			W -= v.W;
			return *this;
		}

		Vector4t& operator *= (const Vector4t& v)
		{
			X *= v.X;
			Y *= v.Y;
			Z *= v.Z;
			W *= v.W;
			return *this;
		}

		Vector4t& operator /= (const Vector4t& v)
		{
			X /= v.X;
			Y /= v.Y;
			Z /= v.Z;
			W /= v.W;
			return *this;
		}

		Vector4t& operator += (const T& s)
		{
			X += s;
			Y += s;
			Z += s;
			W += s;
			return *this;
		}

		Vector4t& operator -= (const T& s)
		{
			X -= s;
			Y -= s;
			Z -= s;
			W -= s;
			return *this;
		}

		Vector4t& operator *= (const T& s)
		{
			X *= s;
			Y *= s;
			Z *= s;
			W *= s;
			return *this;
		}

		Vector4t& operator /= (const T& s)
		{
			X /= s;
			Y /= s;
			Z /= s;
			W /= s;
			return *this;
		}
	};

	/**
		\addtogroup Math
		@{
	*/

	typedef Vector4t<float> Vector4;
	typedef Vector4t<double> Vector4d;
	typedef Vector4t<int> Vector4i;
	typedef Vector4t<unsigned int> Vector4u;
	typedef Vector4t<bool> Vector4b;

	typedef Vector4t<Int64> Vector4i64;
	typedef Vector4t<Int32> Vector4i32;
	typedef Vector4t<Int16> Vector4i16;
	typedef Vector4t<Int8> Vector4i8;
	typedef Vector4t<UInt64> Vector4u64;
	typedef Vector4t<UInt32> Vector4u32;
	typedef Vector4t<UInt16> Vector4u16;
	typedef Vector4t<UInt8> Vector4u8;

	static float Dot(const Vector4& v1, const Vector4& v2)
	{
		return v1.X * v2.X + v1.Y * v2.Y + v1.Z * v2.Z + v1.W * v2.W;
	}

	static float LengthSquared(const Vector4& v)
	{
		return Dot(v, v);
	}

	static float Length(const Vector4& v)
	{
		return Sqrt(Dot(v, v));
	}

	static Vector4 Normalize(const Vector4& v)
	{
		return v / Length(v);
	}

	/** @} */
}

#endif
