#ifndef __XLI_VECTOR3_H__
#define __XLI_VECTOR3_H__

#include <Xli/Vector2.h>

namespace Xli
{
    /**
        \ingroup XliCoreMath
    */
    template <typename T> class Vector3t
    {
    public:
        union
        {
            struct { T X, Y, Z; };
            struct { T R, G, B; };
            T Data[3];
        };

        operator T* ()
        {
            return Data;
        }

        operator const T* () const
        {
            return Data;
        }

        String ToString() const
        {
            return String(X) + ", " + Y + ", " + Z;
        }

        Vector2t<T> XY() const
        {
            return Vector2t<T>(X, Y);
        }

        Vector3t(const Vector2t<T>& xy, T z)
        {
            X = xy.X;
            Y = xy.Y;
            Z = z;
        }

        Vector3t()
        {
        }

        Vector3t(const T& x, const T& y, const T& z)
        {
            X = x;
            Y = y;
            Z = z;
        }

        Vector3t(const Vector3t& v)
        {
            X = v.X;
            Y = v.Y;
            Z = v.Z;
        }

        template <typename U> 
        explicit Vector3t(const Vector3t<U>& v)
        {
            X = (T)v.X;
            Y = (T)v.Y;
            Z = (T)v.Z;
        }

        bool operator == (const Vector3t& v) const
        {
            return (X == v.X) && (Y == v.Y) && (Z == v.Z);
        }

        bool operator != (const Vector3t& v) const
        {
            return !(*this == v);
        }

        Vector3t& operator = (const Vector3t& v)
        {
            X = v.X;
            Y = v.Y;
            Z = v.Z;
            return *this;
        }

        Vector3t operator - () const
        {
            Vector3t r;
            r.X = -X;
            r.Y = -Y;
            r.Z = -Z;
            return r;
        }

        Vector3t operator + (const Vector3t& v) const
        {
            Vector3t r;
            r.X = X + v.X;
            r.Y = Y + v.Y;
            r.Z = Z + v.Z;
            return r;
        }

        Vector3t operator - (const Vector3t& v) const
        {
            Vector3t r;
            r.X = X - v.X;
            r.Y = Y - v.Y;
            r.Z = Z - v.Z;
            return r;
        }

        Vector3t operator * (const Vector3t& v) const
        {
            Vector3t r;
            r.X = X * v.X;
            r.Y = Y * v.Y;
            r.Z = Z * v.Z;
            return r;
        }

        Vector3t operator / (const Vector3t& v) const
        {
            Vector3t r;
            r.X = X / v.X;
            r.Y = Y / v.Y;
            r.Z = Z / v.Z;
            return r;
        }

        Vector3t operator + (const T& s) const
        {
            Vector3t r;
            r.X = X + s;
            r.Y = Y + s;
            r.Z = Z + s;
            return r;
        }

        Vector3t operator - (const T& s) const
        {
            Vector3t r;
            r.X = X - s;
            r.Y = Y - s;
            r.Z = Z - s;
            return r;
        }

        Vector3t operator * (const T& s) const
        {
            Vector3t r;
            r.X = X * s;
            r.Y = Y * s;
            r.Z = Z * s;
            return r;
        }

        Vector3t operator / (const T& s) const
        {
            Vector3t r;
            r.X = X / s;
            r.Y = Y / s;
            r.Z = Z / s;
            return r;
        }

        Vector3t& operator += (const Vector3t& v)
        {
            X += v.X;
            Y += v.Y;
            Z += v.Z;
            return *this;
        }

        Vector3t& operator -= (const Vector3t& v)
        {
            X -= v.X;
            Y -= v.Y;
            Z -= v.Z;
            return *this;
        }

        Vector3t& operator *= (const Vector3t& v)
        {
            X *= v.X;
            Y *= v.Y;
            Z *= v.Z;
            return *this;
        }

        Vector3t& operator /= (const Vector3t& v)
        {
            X /= v.X;
            Y /= v.Y;
            Z /= v.Z;
            return *this;
        }

        Vector3t& operator += (const T& s)
        {
            X += s;
            Y += s;
            Z += s;
            return *this;
        }

        Vector3t& operator -= (const T& s)
        {
            X -= s;
            Y -= s;
            Z -= s;
            return *this;
        }

        Vector3t& operator *= (const T& s)
        {
            X *= s;
            Y *= s;
            Z *= s;
            return *this;
        }

        Vector3t& operator /= (const T& s)
        {
            X /= s;
            Y /= s;
            Z /= s;
            return *this;
        }
    };

    /**
        \addtogroup XliCoreMath
        @{
    */

    typedef Vector3t<float> Vector3;
    typedef Vector3t<double> Vector3d;
    typedef Vector3t<int> Vector3i;
    typedef Vector3t<unsigned int> Vector3u;

    typedef Vector3t<Int64> Vector3i64;
    typedef Vector3t<Int32> Vector3i32;
    typedef Vector3t<Int16> Vector3i16;
    typedef Vector3t<Int8> Vector3i8;
    typedef Vector3t<UInt64> Vector3u64;
    typedef Vector3t<UInt32> Vector3u32;
    typedef Vector3t<UInt16> Vector3u16;
    typedef Vector3t<UInt8> Vector3u8;

    XLI_INLINE Vector3 Cross(const Vector3& v1, const Vector3& v2)
    {
        return Vector3(v1.Y * v2.Z - v1.Z * v2.Y,
                       v1.Z * v2.X - v1.X * v2.Z,
                       v1.X * v2.Y - v1.Y * v2.X);
    }

    XLI_INLINE float Dot(const Vector3& v1, const Vector3& v2)
    {
        return v1.X * v2.X + v1.Y * v2.Y + v1.Z * v2.Z;
    }

    XLI_INLINE float LengthSquared(const Vector3& v)
    {
        return Dot(v, v);
    }

    XLI_INLINE float Length(const Vector3& v)
    {
        return Sqrt(Dot(v, v));
    }

    XLI_INLINE Vector3 Normalize(const Vector3& v)
    {
        return v / Length(v);
    }

    /** @} */
}

#endif
