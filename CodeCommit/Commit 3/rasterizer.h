#ifndef __RASTERIZER_H__
#define __RASTERIZER_H__
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
class Rasterizer
{
public:
    //画线
    void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
    //计算质心坐标
    Vec3f barycentric(Vec3f A, Vec3f B,Vec3f C, Vec3f P);
    //模型变换矩阵
    Matrix get_model_matrix(char c,float rotation);
    //摄像机变换矩阵
    Matrix get_camera_matrix(Vec3f camera, Vec3f center, Vec3f up);
    //投影变换矩阵
    Matrix get_projection_matrix(Vec3f camera, Vec3f center);
    //视角变换矩阵
    Matrix get_viewport_matrix(int width, int height, int depth);
    //用画线法绘制三角形  顶点坐标，与之对应的uv坐标，TGA指针，亮度，zbuffer
    void triangle_line(Vec3f* pts, Vec2i *uv, Model *model, TGAImage &image ,float intensity,int *zbuffer);
    //用包围盒法绘制三角形坐标数组 顶点坐标，与之对应的uv坐标，TGA指针，亮度，zbuffer
    void triangle_box(Vec3f *pts, Vec2i *uv, Model *model, TGAImage &image, float intensity, int *zbuffer);
    
};

#endif //__RASTERIZER_H__