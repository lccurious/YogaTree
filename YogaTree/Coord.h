//
// Created by peo on 18-1-6.
//

#pragma once

#include "Types.h"
#include <limits>
#include <array>
#include <cassert>

template<typename T>
class Vec3 {
public:
    using ValueType = T;

    Vec3() {}

    Vec3(T x, T y, T z)
    {
        this->mm[0] = x;
        this->mm[1] = y;
        this->mm[2] = z;
    }

    T x() { return this->mm[0]; }
    T y() { return this->mm[1]; }
    T z() { return this->mm[2]; }

    /// copy of other value
    Vec3(Vec3* other)
    {
        this->mm[0] = other->x();
        this->mm[1] = other->y();
        this->mm[2] = other->z();
    }

    T* asPointer() { return this->mm; }
    const T* asPointer() const { return this->mm; }

    T& operator()(int i) { return this->mm[i]; }
    T operator()(int i) const { return this-> mm[i]; }

    T operator[](int i) const { return mm[i]; }
    T& operator[](int i) { return mm[i]; }

protected:
    T mm[3];
};

/// help to manage a steady coordinate system
class Coord {
public:

    using Vec3i = Vec3<Int32>;
    using Vec3I = Vec3<Index32>;

    using ValueType = Int32;
    using Limits = std::numeric_limits<ValueType>;

    Coord(): mVec{{0, 0, 0}} {}
    explicit Coord(Int32 xyz): mVec{{xyz, xyz, xyz}} {}
    Coord(Int32 x, Int32 y, Int32 z): mVec{{x, y, z}} {}
    explicit Coord(const Vec3i& v): mVec{{v[0], v[1], v[2]}} {}
    ~Coord() = default;

    void setX(Int32 x) { mVec[0] = x; }
    void setY(Int32 y) { mVec[1] = y; }
    void setZ(Int32 z) { mVec[2] = z; }

    void setWidth(Index width) { _width = width; }
    void setHeight(Index height) { _height = height; }
    Index getWidth() { return _width; }
    Index getHeight() { return _height; }

    /// @brief Return the smallest possible coordinate
    static Coord min() { return Coord(Limits::min()); }

    /// @brief Return the largest possible coordinate
    static Coord max() { return Coord(Limits::max()); }

    Coord offsetBy(Int32 dx, Int32 dy, Int32 dz) const
    {
        return Coord(mVec[0] + dx, mVec[1] + dy, mVec[2] + dz);
    }
    Coord offsetBy(Int32 n) const { return offsetBy(n, n, n); }

    Coord& operator += (const Coord& rhs)
    {
        mVec[0] += rhs[0];
        mVec[1] += rhs[1];
        mVec[2] += rhs[2];
        return *this;
    }
    Coord& operator -= (const Coord& rhs)
    {
        mVec[0] -= rhs[0];
        mVec[1] -= rhs[1];
        mVec[2] -= rhs[2];
        return *this;
    }
    Coord operator + (const Coord& rhs) const
    {
        return Coord(mVec[0] + rhs[0], mVec[1] + rhs[1], mVec[2] + rhs[2]);
    }
    Coord operator - (const Coord& rhs) const
    {
        return Coord(mVec[0] - rhs[0], mVec[1] - rhs[1], mVec[2] - rhs[2]);
    }
    Coord operator - () const { return Coord(-mVec[0], -mVec[1], -mVec[2]); }

    Coord  operator>> (size_t n) const { return Coord(mVec[0]>>n, mVec[1]>>n, mVec[2]>>n); }
    Coord  operator<< (size_t n) const { return Coord(mVec[0]<<n, mVec[1]<<n, mVec[2]<<n); }
    Coord& operator<<=(size_t n) { mVec[0]<<=n; mVec[1]<<=n; mVec[2]<<=n; return *this; }
    Coord& operator>>=(size_t n) { mVec[0]>>=n; mVec[1]>>=n; mVec[2]>>=n; return *this; }
    Coord  operator&  (Int32 n) const { return Coord(mVec[0] & n, mVec[1] & n, mVec[2] & n); }
    Coord  operator|  (Int32 n) const { return Coord(mVec[0] | n, mVec[1] | n, mVec[2] | n); }
    Coord& operator&= (Int32 n) { mVec[0]&=n; mVec[1]&=n; mVec[2]&=n; return *this; }
    Coord& operator|= (Int32 n) { mVec[0]|=n; mVec[1]|=n; mVec[2]|=n; return *this; }

    Int32 x() const { return mVec[0]; }
    Int32 y() const { return mVec[1]; }
    Int32 z() const { return mVec[2]; }
    Int32 operator[](size_t i) const { assert(i < 3); return mVec[i]; }
    Int32& x() { return mVec[0]; }
    Int32& y() { return mVec[1]; }
    Int32& z() { return mVec[2]; }
    Int32& operator[](size_t i) { assert(i < 3); return mVec[i]; }

    bool operator==(const Coord& rhs) const
    {
        return (mVec[0] == rhs.mVec[0] && mVec[1] == rhs.mVec[1] && mVec[2] == rhs.mVec[2]);
    }
    bool operator!=(const Coord& rhs) const { return !(*this == rhs); }

    /// Lexicographic less than
    bool operator<(const Coord& rhs) const
    {
        return this->x() < rhs.x() ? true : this->x() > rhs.x() ? false
             : this->y() < rhs.y() ? true : this->y() > rhs.y() ? false
             : this->z() < rhs.z() ? true : false;
    }
    /// Lexicographic less than or equal to
    bool operator<=(const Coord& rhs) const
    {
        return this->x() < rhs.x() ? true : this->x() > rhs.x() ? false
             : this->y() < rhs.y() ? true : this->y() > rhs.y() ? false
             : this->z() <=rhs.z() ? true : false;
    }
    /// Lexicographic greater than
    bool operator>(const Coord& rhs) const { return !(*this <= rhs); }
    /// Lexicographic greater than or equal to
    bool operator>=(const Coord& rhs) const { return !(*this < rhs); }

    void asXYZ(Int32& x, Int32& y, Int32& z) const { x = mVec[0]; y = mVec[1]; z = mVec[2]; }

private:
    Index _width, _height;
    std::array<Int32, 3> mVec;
};
