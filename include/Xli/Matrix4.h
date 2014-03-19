#ifndef __XLI_MATRIX4_H__
#define __XLI_MATRIX4_H__

#include <Xli/Matrix3.h>

namespace Xli
{
    /**
        \ingroup XliCoreMath
    */
    class Matrix4
    {
    public:
        float Data[16];

        Matrix4();
        Matrix4(const Matrix4& m);
        explicit Matrix4(const float* values);

        Matrix4(float m11, float m12, float m13, float m14, 
                float m21, float m22, float m23, float m24, 
                float m31, float m32, float m33, float m34, 
                float m41, float m42, float m43, float m44);

        static const Matrix4& Identity();

        static Matrix4 Translation(float x, float y, float z);
        static Matrix4 Translation(const Vector3& v);

        static Matrix4 RotationX(float angleRadians);
        static Matrix4 RotationY(float angleRadians);
        static Matrix4 RotationZ(float angleRadians);

        static Matrix4 Rotation(const Quaternion& rotation);
        static Matrix4 Rotation(const Vector3& axis, float angleRadians);
        static Matrix4 Rotation(float x, float y, float z, float angleRadians);

        static Matrix4 Scaling(float x, float y, float z);
        static Matrix4 Scaling(const Vector3& v);
        static Matrix4 Scaling(float s);

        static Matrix4 Transpose(const Matrix4& m);
        static bool Invert(const Matrix4& m, Matrix4& outResult);
        static bool Decompose(const Matrix4& m, Quaternion& outRotation, Vector3& outScale, Vector3& outTranslation); 

        operator float* ();
        operator const float* () const;
        
        String ToString() const;
        Matrix3 GetUpperLeft3x3() const;
        Vector3 GetTranslation() const;
        Vector3 GetScaling() const;

        void Transpose();
        bool Invert();

        Matrix4& operator = (const Matrix4& m);

        Matrix4 operator + (const Matrix4& m) const;
        Matrix4 operator - (const Matrix4& m) const;

        Matrix4 operator + (float s) const;
        Matrix4 operator - (float s) const;
        Matrix4 operator * (float s) const;

        Matrix4& operator += (const Matrix4& m);
        Matrix4& operator -= (const Matrix4& m);

        Matrix4& operator += (float s);
        Matrix4& operator -= (float s);
        Matrix4& operator *= (float s);

        Vector3 operator * (const Vector3& v) const;
        Vector4 operator * (const Vector4& v) const;
        Matrix4 operator * (const Matrix4& m) const;
        Matrix4& operator *= (const Matrix4& m);
    };
}

#endif
