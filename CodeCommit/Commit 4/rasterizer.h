#ifndef __RASTERIZER_H__
#define __RASTERIZER_H__
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include "shader.h"
class Rasterizer
{
public:
    //画线
    void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
    //计算质心坐标
    Vec3f barycentric(Vec4f A, Vec4f B,Vec4f C, Vec2i P);
    //用包围盒法绘制三角形  顶点坐标，着色器，TGA指针，zbuffer
    void triangle_box(Vec4f* pts, Shader &shader, TGAImage &image ,TGAImage &zbuffer);
    
};

#endif //__RASTERIZER_H__