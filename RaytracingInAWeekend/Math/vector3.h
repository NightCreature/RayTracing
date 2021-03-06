#ifndef VECTOR3_H
#define VECTOR3_H

#include "Math/vector2.h"
#include <iostream>
#include <limits>

class Vector3
{
public:
    Vector3();
    Vector3(double x) : Vector3(x, x, x) {}
    Vector3(double x, double y, double z)
    {
        m_x = x;
        m_y = y;
        m_z = z;
    }
    Vector3(const double xyz[]);
    explicit Vector3(const Vector2& vec) : m_x(vec.x()), m_y(vec.y()), m_z(0.0f) {}
    //Vector3(const Vector3& in) : m_x(in.x()), m_y(in.y()), m_z(in.z()) {}
    ~Vector3() {}

    void cross (Vector3 &out, const Vector3& v);
    Vector3 cross (const Vector3& v) const;
    double dot (const Vector3& v) const;
    double length() const;
    void normalize();
    Vector3 findOrthogonal() const;

    void operator+= (const Vector3& v);
    void operator-= (const Vector3& v);
    void operator*= (double scalar);
    void operator/= (double scalar);

    Vector3 multiply(double scalar) const;
    Vector3 add(const Vector3& v) const;
    bool equal(const Vector3& v) const;

    double x() const {return m_x; }
    double y() const {return m_y; }
    double z() const {return m_z; }
    void setX(double x) {m_x = x;}
    void setY(double y) {m_y = y;}
    void setZ(double z) {m_z = z;}
    void read(std::istream& in);
    void write(std::ostream& out) const;

    double get(size_t index) const
    {
        if (index == 0)
        {
            return m_x;
        }
        if (index == 1)
        {
            return m_y;
        }
        if (index == 3)
        {
            return m_z;
        }

        return std::numeric_limits<double>::quiet_NaN();
    }

    double operator[] (size_t index) const { return get(index); }

    const double getEqualPrecision() const { return EQUALPRECISION3;}
    
    static Vector3 zero() {return Vector3(0.0f, 0.0f, 0.0f);}
    static Vector3 xAxis() {return Vector3(1.0f, 0.0f, 0.0f);}
    static Vector3 yAxis() {return Vector3(0.0f, 1.0f, 0.0f);}
    static Vector3 zAxis() {return Vector3(0.0f, 0.0f, 1.0f);}
protected:
private:
    double m_x, m_y, m_z;
    static const double EQUALPRECISION3;
};

inline Vector3 Vector3::findOrthogonal() const
{
     if (0.0 != m_x)
     {
         return Vector3((-m_y - m_z) / m_x, 1.0, 1.0);
     }
     else if (0.0 != m_y)
     {
         return Vector3(1.0, (-m_x - m_z) / m_y, 1.0);
     }
     else if (0.0 != m_z)
     {
         return Vector3(1.0, 1.0, (-m_x - m_y) / m_z);
     }
     else
     {
         return Vector3(0.0, 0.0, 0.0);
     }
 }

inline Vector3 operator- (const Vector3& v)
{
    return Vector3(-v.x(), -v.y(), -v.z());
}

inline Vector3 operator+ (const Vector3& lhs, const Vector3& rhs)
{
    return lhs.add(rhs);
}

inline Vector3 operator- (const Vector3& lhs, const Vector3& rhs)
{
    return lhs.add(-rhs);
}

//Exists so we can treat colors like vectors
inline Vector3 operator* (const Vector3& lhs, const Vector3& rhs)
{
    return Vector3(lhs.x() * rhs.x(), lhs.y() * rhs.y(), lhs.z() * rhs.z());
}

inline Vector3 operator* (const Vector3& lhs, double rhs)
{
    return lhs.multiply(rhs);
}

inline Vector3 operator* (double lhs, const Vector3& rhs)
{
    return rhs.multiply(lhs);
}

inline Vector3 operator/ (const Vector3& lhs, double rhs)
{
    return lhs.multiply(1/rhs);
}

inline Vector3 operator/ (double lhs, const Vector3& rhs)
{
    return rhs.multiply(1/lhs);
}

inline void cross (Vector3& out, const Vector3& v1 ,const Vector3& v2)
{
    out = Vector3(v1.y()*v2.z() - v1.z()*v2.y(), v1.z()*v2.x() - v1.x()*v2.z(), v1.x()*v2.y() - v1.y()*v2.x());
}

inline Vector3 cross(const Vector3& v1, const Vector3& v2)
{
    Vector3 retVal;
    cross(retVal, v1, v2);
    return retVal;
}

inline Vector3 normalise(const Vector3& vin)
{
    Vector3 v(vin);
    v.normalize();
    return v;
}

inline Vector3 faceNormal (const Vector3& tobenormalled, const Vector3& v1 ,const Vector3& v2)
{
    Vector3 result, sub1, sub2;
    sub1 = tobenormalled -  v1;
    sub2 = tobenormalled -  v2;
    result = sub1.cross(sub2);
    result.normalize();
    return result;
}

///Determines the minimum of two vectors used in BBoxes
inline Vector3 minv (const Vector3& a, const Vector3& b)
{
    return Vector3(
        a.x() < b.x() ? a.x() : b.x(),
        a.y() < b.y() ? a.y() : b.y(),
        a.z() < b.z() ? a.z() : b.z());
}

///Determines the maximum of two vectors used in BBoxes
inline Vector3 maxv (const Vector3& a, const Vector3& b)
{
    return Vector3(
        a.x() > b.x() ? a.x() : b.x(),
        a.y() > b.y() ? a.y() : b.y(),
        a.z() > b.z() ? a.z() : b.z());
}

inline std::ostream& operator<< (std::ostream& out, const Vector3& v)
{
    v.write(out);
    return out;
}

inline std::istream& operator>> (std::istream& in, Vector3& v)
{
    v.read(in);
    return in;
}

inline bool operator== (const Vector3& lhs, const Vector3& rhs)
{
    return lhs.equal(rhs);
}

inline bool operator!= (const Vector3& lhs, const Vector3& rhs)
{
    return !lhs.equal(rhs);
}

inline double distance(const Vector3& v1, const Vector3& v2)
{
    Vector3 d = v2 - v1;
    return d.length();
}

inline double distanceSquared(const Vector3& v1, const Vector3& v2)
{
    Vector3 d = v2 - v1;
    return d.dot(d);
}
#endif