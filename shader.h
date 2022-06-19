#ifndef __SHADER_H__
#define __SHADER_H__
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

extern Model *model;

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
    //光照强度
    Vec3f light_dir;

    //顶点着色器
    virtual Vec4f vertex(int iface,int jvertex) = 0;
    //片元着色器
    virtual bool fragment(Vec3f bc_screen, TGAColor &color) = 0;

    Matrix get_model_matrix(char c,float rotation);
    Matrix get_camera_matrix(Vec3f camera, Vec3f center, Vec3f up);
    Matrix get_projection_matrix(Vec3f camera, Vec3f center);
    Matrix get_viewport_matrix(int width, int height, int depth);
};


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
#endif //__SHADER_H__