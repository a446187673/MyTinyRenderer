#ifndef __SHADER_H__
#define __SHADER_H__
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

extern Model *model;
extern Vec3f camera;
class Shader
{public:
    Shader() = default;
    virtual ~Shader() = default;

    //模型变换矩阵
    Matrix model_matrix;
    //摄像机变换矩阵
    Matrix camera_matrix;
    //投影变换矩阵
    Matrix projection_matrix;
    //视角变换矩阵
    Matrix viewport_matrix;
    //光照方向
    Vec3f light_dir;
    //世界坐标
    Vec3f world_coords[3];

    //顶点着色器
    virtual Vec4f vertex(int iface,int jvertex) = 0;
    //片元着色器
    virtual bool fragment(Vec3f bc_screen, TGAColor &color) = 0;

    Matrix get_model_matrix(char c,float rotation);
    Matrix get_camera_matrix(Vec3f camera, Vec3f center, Vec3f up);
    Matrix get_projection_matrix(Vec3f camera, Vec3f center);
    Matrix get_viewport_matrix(int width, int height, int depth);
};

//高洛德着色器
class GouraudShader : public Shader
{public:
    GouraudShader() = default;
    virtual ~GouraudShader() = default;
    //储存uv坐标的数组
    Vec2f uv[3];
    //光照强度
    Vec3f intensity;
    //顶点着色器，接收两个变量（面序号，顶点序号）
    virtual Vec4f vertex(int iface,int jvertex);
    //片元着色器，接收两个变量（质心坐标，像素点颜色）
    virtual bool fragment(Vec3f bc_screen, TGAColor &color);
};
//卡通着色器
class ToonShader : public Shader
{public:
    ToonShader() = default;
    virtual ~ToonShader() = default;
    //光照强度
    Vec3f intensity;
    //顶点着色器，接收两个变量（面序号，顶点序号）
    virtual Vec4f vertex(int iface,int jvertex);
    //片元着色器，接收两个变量（质心坐标，像素点颜色）
    virtual bool fragment(Vec3f bc_screen, TGAColor &color);
};
//平面着色器
class FlatShader : public Shader
{public:
    FlatShader() = default;
    virtual ~FlatShader() = default;
    //储存uv坐标的数组
    Vec2f uv[3];
    //经过前三个矩阵变化后的的世界坐标
    Vec3f varying_tri[3];
    //光照强度
    Vec3f intensity;
    //顶点着色器，接收两个变量（面序号，顶点序号）
    virtual Vec4f vertex(int iface,int jvertex);
    //片元着色器，接收两个变量（质心坐标，像素点颜色）
    virtual bool fragment(Vec3f bc_screen, TGAColor &color);
};
//冯氏着色器
class PhongShader : public Shader
{public:
    PhongShader() = default;
    virtual ~PhongShader() = default;
    //经过前三个矩阵变化后的的世界坐标
    Vec3f varying_tri[3];
    //储存uv坐标的数组
    Vec2f uv[3];
    //顶点法向量
    Vec3f normal[3];
    //光照强度
    Vec3f intensity;
    //顶点着色器，接收两个变量（面序号，顶点序号）
    virtual Vec4f vertex(int iface,int jvertex);
    //片元着色器，接收两个变量（质心坐标，像素点颜色）
    virtual bool fragment(Vec3f bc_screen, TGAColor &color);
    //获取TBN矩阵
    Matrix get_TBN(Vec3f bc_screen);
};
#endif //__SHADER_H__