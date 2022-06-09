#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include<iostream>
#include <cmath>
#include<cassert>

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

typedef Vec2<float> Vec2f;
typedef Vec2<int>   Vec2i;
typedef Vec3<float> Vec3f;
typedef Vec3<int>   Vec3i;

template <> template <> Vec3<int>::Vec3(const Vec3<float> &v);
template <> template <> Vec3<float>::Vec3(const Vec3<int> &v);

//重载输入、输出运算符
template <typename T> std::ostream& operator<<(std::ostream& s, Vec2<T>& v) {
    s << "(" << v.x << ", " << v.y << ")\n";
    return s;
}

template <typename T> std::ostream& operator<<(std::ostream& s, Vec3<T>& v) {
    s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
    return s;
}

//////////////////////////////////////////////////////////////////////////////////////////////

const int DEFAULT_ALLOC=4;

class Matrix {
    std::vector<std::vector<float> > m;
    int rows, cols;
public:
    Matrix(int r=DEFAULT_ALLOC, int c=DEFAULT_ALLOC);
    inline int nrows();
    inline int ncols();

    static Matrix identity(int dimensions);
    std::vector<float>& operator[](const int i);
    Matrix operator*(const Matrix& a);
    Matrix transpose();
    Matrix inverse();
    
    //4d->3d
    //除以最后一个分量，当最后一个分量为0，表示向量。不为0，表示坐标
    static Vec3f m2v(Matrix m);
    //3d->4d
    //添加一个1表示坐标
    static Matrix v2m(Vec3f v);
    //视角矩阵
    //将物体x，y坐标(-1,1)转到屏幕坐标(100,700) 1/8width ~ 7/8width
    //zbuffer(-1,1)转换到0~255
    static Matrix viewport(int x, int y, int w, int h, int depth);
    
    friend std::ostream& operator<<(std::ostream& s, Matrix& m);
};

/////////////////////////////////////////////////////////////////////////////////////////////


#endif //__GEOMETRY_H__
