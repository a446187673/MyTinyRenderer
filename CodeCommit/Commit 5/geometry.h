#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include<iostream>
#include <cmath>
#include<cassert>
class Matrix;
template <typename T> struct Vec2 {
    T x, y;
    //构造函数
    Vec2<T>() : x(0), y(0) {}
    Vec2<T>(T _x, T _y) : x(_x), y(_y) {}
    //拷贝构造函数
    Vec2<T>(const Vec2<T> &v) : x(0), y(0) { *this = v; }
    //重载=运算符
    Vec2<T> & operator=(const Vec2<T> &v) 
    {
        if (this != &v) {
            x = v.x;
            y = v.y;
        }
        return *this;
    }
    //向量相加、相减、相乘
    inline Vec2<T> operator +(const Vec2<T> &v) const { return Vec2<T>(x+v.x, y+v.y); }
    inline Vec2<T> operator -(const Vec2<T> &v) const { return Vec2<T>(x-v.x, y-v.y); }
    inline Vec2<T> operator *(float f)          const { return Vec2<T>(x*f, y*f); }
    //通过下标访问
    T& operator[](const int i) 
    { 
        //若数字>1则报错，下标越界
        assert(i < 2);
        return i <= 0 ? x : y;
    }
    template <typename > friend std::ostream& operator<<(std::ostream& s, Vec2<T>& v);
};

template <typename T> struct Vec3 {
    T x, y, z;
    //构造函数
    Vec3<T>() : x(0), y(0), z(0) {}
    Vec3<T>(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}
    Vec3<T>(Matrix m);
    template <typename u> Vec3<T>(const Vec3<u> &v);
    //拷贝构造函数
    Vec3<T>(const Vec3<T> &v) : x(T()), y(T()), z(T()) { *this = v; }
    //重载=运算符
    Vec3<T> & operator =(const Vec3<T> &v) {
        if (this != &v) {
            x = v.x;
            y = v.y;
            z = v.z;
        }
        return *this;
    }
    //向量叉乘、相加、相减、以及乘以常数
    inline Vec3<T> operator ^(const Vec3<T> &v) const { return Vec3<T>(y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x); }
    inline Vec3<T> operator +(const Vec3<T> &v) const { return Vec3<T>(x+v.x, y+v.y, z+v.z); }
    inline Vec3<T> operator -(const Vec3<T> &v) const { return Vec3<T>(x-v.x, y-v.y, z-v.z); }
    inline Vec3<T> operator *(float f)          const { return Vec3<T>(x*f, y*f, z*f); }
    inline T       operator *(const Vec3<T> &v) const { return x*v.x + y*v.y + z*v.z; }
    //向量的模
    float norm () const { return std::sqrt(x*x+y*y+z*z); }
    //归一化操作
    Vec3<T> & normalize(T l=1) { *this = (*this)*(l/norm()); return *this; }
    T& operator[](const int i)
    {
        //若数字>2则报错，下标越界
        assert(i < 3);
        if(i <= 0) 
            return x; 
        else if(i == 1) 
            return y; 
        else
            return z; 
    }
    template <typename > friend std::ostream& operator<<(std::ostream& s, Vec3<T>& v);
};

template <typename T> struct Vec4 {
    T x, y, z, w;
    //构造函数
    Vec4<T>() : x(0), y(0), z(0), w(0) {}
    Vec4<T>(T _x, T _y, T _z, T _w) : x(_x), y(_y), z(_z), w(_w) {}
    Vec4<T>(Matrix m);
    template <typename u> Vec4<T>(const Vec4<u> &v);
    //拷贝构造函数
    Vec4<T>(const Vec4<T> &v) : x(T()), y(T()), z(T()), w(T()) { *this = v; }
    //重载=运算符
    Vec4<T> & operator =(const Vec4<T> &v) {
        if (this != &v) {
            x = v.x;
            y = v.y;
            z = v.z;
            w = v.w;
        }
        return *this;
    }
    //向量相加、相减、以及乘以常数
    inline Vec4<T> operator +(const Vec4<T> &v) const { return Vec4<T>(x+v.x, y+v.y, z+v.z, w+v.w); }
    inline Vec4<T> operator -(const Vec4<T> &v) const { return Vec4<T>(x-v.x, y-v.y, z-v.z, w-v.w); }
    inline Vec4<T> operator *(float f)          const { return Vec4<T>(x*f, y*f, z*f, w*f); }
    inline T       operator *(const Vec4<T> &v) const { return x*v.x + y*v.y + z*v.z + w*v.w; }
    inline Vec4<T> operator /(float f)          const { return Vec4<T>(x/f, y/f, z/f, w/f); }
    T& operator[](const int i)
    {
        //若数字>2则报错，下标越界
        assert(i < 4);
        if(i <= 0) 
            return x; 
        else if(i == 1) 
            return y; 
        else if(i == 2)
            return z; 
        else
            return w;
    }
    template <typename > friend std::ostream& operator<<(std::ostream& s, Vec4<T>& v);
};

typedef Vec2<float> Vec2f;
typedef Vec2<int>   Vec2i;
typedef Vec3<float> Vec3f;
typedef Vec3<int>   Vec3i;
typedef Vec4<float> Vec4f;
typedef Vec4<int>   Vec4i;

template <> template <> Vec3<int>::Vec3(const Vec3<float> &v);
template <> template <> Vec3<float>::Vec3(const Vec3<int> &v);
template <> template <> Vec4<int>::Vec4(const Vec4<float> &v);
template <> template <> Vec4<float>::Vec4(const Vec4<int> &v);

//重载输入、输出运算符
template <typename T> std::ostream& operator<<(std::ostream& s, Vec2<T>& v) {
    s << "(" << v.x << ", " << v.y << ")\n";
    return s;
}

template <typename T> std::ostream& operator<<(std::ostream& s, Vec3<T>& v) {
    s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
    return s;
}

template <typename T> std::ostream& operator<<(std::ostream& s, Vec4<T>& v) {
    s << "(" << v.x << ", " << v.y << ", " << v.z << ", " <<v.w<< ")\n";
    return s;
}
//////////////////////////////////////////////////////////////////////////////////////////////

const int DEFAULT_ALLOC=4;

class Matrix {
    std::vector<std::vector<float> > m;
    int rows, cols;
public:
    Matrix(int r=DEFAULT_ALLOC, int c=DEFAULT_ALLOC);
    Matrix(Vec3f v);
    Matrix(Vec4f v);
    inline int nrows();
    inline int ncols();

    static Matrix identity(int dimensions);
    std::vector<float>& operator[](const int i);
    Matrix operator*(const Matrix& a);
    Matrix operator*(const float& num);
    Vec4f operator*(Vec4f& vec);
    Vec3f operator*(Vec3f& vec);
    Matrix transpose();
    Matrix inverse();
    
    friend std::ostream& operator<<(std::ostream& s, Matrix& m);
    friend void operator<<(Matrix& mat ,std::vector<float> vec);
};

/////////////////////////////////////////////////////////////////////////////////////////////


#endif //__GEOMETRY_H__
