#include <vector>
#include <cassert>
#include <cmath>
#include <iostream>
#include "geometry.h"
//矩阵与向量的转换
template <> Vec3<float>::Vec3(Matrix m) : x(m[0][0]), y(m[1][0]), z(m[2][0]) {}
template <> Vec4<float>::Vec4(Matrix m) : x(m[0][0]), y(m[1][0]), z(m[2][0]), w(m[3][0]) {}

template <> template <> Vec3<int>::Vec3<>(const Vec3<float> &v) : x(int(v.x+.5)), y(int(v.y+.5)), z(int(v.z+.5)) {}
template <> template <> Vec3<float>::Vec3<>(const Vec3<int> &v) : x(v.x), y(v.y), z(v.z) {}

template <> template <> Vec4<int>::Vec4<>(const Vec4<float> &v) : x(int(v.x+.5)), y(int(v.y+.5)), z(int(v.z+.5)), w(int(v.w+.5)) {}
template <> template <> Vec4<float>::Vec4<>(const Vec4<int> &v) : x(v.x), y(v.y), z(v.z), w(v.w) {}

Matrix::Matrix(Vec3f v) : m(std::vector<std::vector<float> >(3, std::vector<float>(1, 1.0f))), rows(3), cols(1) {
    m[0][0] = v.x;
    m[1][0] = v.y;
    m[2][0] = v.z;
}
Matrix::Matrix(Vec4f v) : m(std::vector<std::vector<float> >(4, std::vector<float>(1, 1.0f))), rows(4), cols(1) {
    m[0][0] = v.x;
    m[1][0] = v.y;
    m[2][0] = v.z;
    m[3][0] = v.w;
}

Matrix::Matrix(int r, int c) : m(std::vector<std::vector<float> >(r, std::vector<float>(c, 0.f))), rows(r), cols(c) { }

int Matrix::nrows() {
    return rows;
}

int Matrix::ncols() {
    return cols;
}

Matrix Matrix::identity(int dimensions) {
    Matrix E(dimensions, dimensions);
    for (int i=0; i<dimensions; i++) {
        for (int j=0; j<dimensions; j++) {
            E[i][j] = (i==j ? 1.f : 0.f);
        }
    }
    return E;
}

std::vector<float>& Matrix::operator[](const int i) {
    assert(i>=0 && i<rows);
    return m[i];
}

Matrix Matrix::operator*(const Matrix& a) {
    assert(cols == a.rows);
    Matrix result(rows, a.cols);
    for (int i=0; i<rows; i++) {
        for (int j=0; j<a.cols; j++) {
            result.m[i][j] = 0.f;
            for (int k=0; k<cols; k++) {
                result.m[i][j] += m[i][k]*a.m[k][j];
            }
        }
    }
    return result;
}
Matrix Matrix::operator*(const float& num){
    Matrix result(cols,rows);
    for (int i=0; i<rows; i++){
        for (int j=0; j<cols; j++){
            result[i][j] = m[i][j] * num;
        }
    }
    return result;
}
Vec4f Matrix::operator*(Vec4f& vec)
{
    Vec4f res;
    if(cols != 4 || rows != 4)
    {
        std::cout<<"该矩阵不为4*4矩阵"<<std::endl;
        return res;
    }
    res.x = m[0][0]*vec.x + m[0][1]*vec.y + m[0][2]*vec.z + m[0][3]*vec.w;
    res.y = m[1][0]*vec.x + m[1][1]*vec.y + m[1][2]*vec.z + m[1][3]*vec.w;
    res.z = m[2][0]*vec.x + m[2][1]*vec.y + m[2][2]*vec.z + m[2][3]*vec.w;
    res.w = m[3][0]*vec.x + m[3][1]*vec.y + m[3][2]*vec.z + m[3][3]*vec.w;
    return res;
}
Vec3f Matrix::operator*(Vec3f& vec)
{
    Vec3f res;
    if(cols != 3 || rows != 3)
    {
        std::cout<<"该矩阵不为3*3矩阵"<<std::endl;
        return res;
    }
    res.x = m[0][0]*vec.x + m[0][1]*vec.y + m[0][2]*vec.z;
    res.y = m[1][0]*vec.x + m[1][1]*vec.y + m[1][2]*vec.z;
    res.z = m[2][0]*vec.x + m[2][1]*vec.y + m[2][2]*vec.z;
    return res;
}

Matrix Matrix::transpose() {
    Matrix result(cols, rows);
    for(int i=0; i<rows; i++)
        for(int j=0; j<cols; j++)
            result[j][i] = m[i][j];
    return result;
}

Matrix Matrix::inverse() {
    assert(rows==cols);
    // augmenting the square matrix with the identity matrix of the same dimensions a => [ai]
    Matrix result(rows, cols*2);
    for(int i=0; i<rows; i++)
        for(int j=0; j<cols; j++)
            result[i][j] = m[i][j];
    for(int i=0; i<rows; i++)
        result[i][i+cols] = 1;
    // first pass
    for (int i=0; i<rows-1; i++) {
        // normalize the first row
        for(int j=result.cols-1; j>=0; j--)
            result[i][j] /= result[i][i];
        for (int k=i+1; k<rows; k++) {
            float coeff = result[k][i];
            for (int j=0; j<result.cols; j++) {
                result[k][j] -= result[i][j]*coeff;
            }
        }
    }
    // normalize the last row
    for(int j=result.cols-1; j>=rows-1; j--)
        result[rows-1][j] /= result[rows-1][rows-1];
    // second pass
    for (int i=rows-1; i>0; i--) {
        for (int k=i-1; k>=0; k--) {
            float coeff = result[k][i];
            for (int j=0; j<result.cols; j++) {
                result[k][j] -= result[i][j]*coeff;
            }
        }
    }
    // cut the identity matrix back
    Matrix truncate(rows, cols);
    for(int i=0; i<rows; i++)
        for(int j=0; j<cols; j++)
            truncate[i][j] = result[i][j+cols];
    return truncate;
}

std::ostream& operator<<(std::ostream& s, Matrix& m) {
    for (int i=0; i<m.nrows(); i++)  {
        for (int j=0; j<m.ncols(); j++) {
            s << m[i][j];
            if (j<m.ncols()-1) s << "\t";
        }
        s << "\n";
    }
    return s;
}
void operator<<(Matrix& mat ,std::vector<float> vec)
{
    for (int i = 0; i < mat.nrows(); i++)
        for (int j = 0; j < mat.ncols(); j++)
            mat[i][j] = vec[i*mat.ncols() + j];
}
