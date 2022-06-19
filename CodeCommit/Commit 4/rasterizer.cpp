#include <limits>
#include "rasterizer.h"
//定义宽度，高度,深度
const int width = 800;
const int height = 800;
const int depth = 255;
//画线
void Rasterizer::line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
{
    bool steep = false; // steep表示斜率是否大于1，false为斜率小于1，即Δx>Δy
    if (std::abs(x0 - x1) < std::abs(y0 - y1))
    { //斜率大于1，steep变为true，交换x0，y0，防止线段不连续
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0 > x1) //保证x1是更大的数，x0是更小的数
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int dx = x1 - x0;
    int dy = y1 - y0;
    int derror = std::abs(dy) * 2; //可以理解为与斜率等于1时的误差
    int error = 0;
    int y = y0;
    for (int x = x0; x <= x1; x++)
    {
        if (steep == false)
            image.set(x, y, color);
        else
            image.set(y, x, color);
        error += derror;
        if (error > dx) //当误差大于一定程度时y+1或-1
        {
            y += y1 > y0 ? 1 : -1;
            error -= dx * 2;
        }
    }
}
//计算质心坐标
Vec3f Rasterizer::barycentric(Vec4f A, Vec4f B,Vec4f C, Vec2i P)
{
        // std::cout << A << std::endl;
        Vec3f s[2];
        //计算[AB,AC,PA]的x分量
        s[0].x = C.x - A.x;
        s[0].y = B.x - A.x;
        s[0].z = A.x - P.x;
        //计算[AB,AC,PA]的y分量
        s[1].x = C.y - A.y;
        s[1].y = B.y - A.y;
        s[1].z = A.y - P.y;
        //[u,v,1]和[AB,AC,PA]对应的x和y向量都垂直，所以叉乘得到k[u,v,1]
        Vec3f u = s[0]^s[1];
        //三点共线时，会导致u.z为0，此时返回(-1,1,1)
        if(std::abs(u.z)>1e-2)
                //若1-u-v，u，v全大于0，表示点在三角形内部
                return Vec3f(1.0f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
        return Vec3f(-1,1,1);
}
//用包围盒法绘制三角形坐标数组 顶点坐标，与之对应的uv坐标，TGA指针，亮度，zbuffer
void Rasterizer::triangle_box(Vec4f* pts, Shader &shader, TGAImage &image ,TGAImage &zbuffer)
{
    Vec2f bboxmin( std::numeric_limits<float>::max(),  std::numeric_limits<float>::max());
    Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
    Vec2f clamp(image.get_width() - 1, image.get_height() - 1);
    //确定三角形的边框
    for (int i = 0; i < 3; i++)
    {
        //确定x轴边框
        bboxmin.x = std::max(0.0f,    std::min(bboxmin.x, pts[i].x/pts[i].w));
        bboxmax.x = std::min(clamp.x, std::max(bboxmax.x, pts[i].x/pts[i].w));
        //确定y轴边框
        bboxmin.y = std::max(0.0f,    std::min(bboxmin.y, pts[i].y/pts[i].w));
        bboxmax.y = std::min(clamp.y, std::max(bboxmax.y, pts[i].y/pts[i].w));
    }
    //当前像素坐标P，颜色color
    Vec2i P;
    TGAColor color;
    //遍历边框中的每一个点
    for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++){
        for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++)
        {
            //质心坐标
            Vec3f c = barycentric(pts[0]/pts[0].w, pts[1]/pts[1].w, pts[2]/pts[2].w, P);
            
            //透视矫正
            Vec3f bc_screen = { 0,0,0 };
            for (int i = 0; i < 3; ++i)
                bc_screen[i] = c[i] / pts[i][3];//求α，β，γ,只需要除以pts第四个分量即可
            float Z_n = 1. / (bc_screen[0] + bc_screen[1] + bc_screen[2]);
            for (int i = 0; i < 3; ++i)
                bc_screen[i] *= Z_n;//求正确透视下插值的系数

            //计算zbuffer，并且每个顶点的z值乘上对应的质心坐标分量
            float P_z = (pts[0].z/pts[0].w) * bc_screen.x + (pts[1].z/pts[1].w) * bc_screen.y + (pts[2].z/pts[2].w) * bc_screen.z;
            int P_depth = std::max(0, std::min(255, int(P_z + 0.5)));
            //如果质心坐标有一个负值，说明点在三角形外。或是该点相比于zbuffer上已有点的位置离屏幕更远。这两种情况都不渲染
            if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0 || zbuffer.get(P.x, P.y).b > P_depth)
                continue;

            bool discard = shader.fragment(bc_screen,color);
            if (discard == false) //查找该像素在zubuffer上的位置，如果更靠近屏幕就渲染
            {
                zbuffer.set(P.x, P.y, TGAColor(P_depth));
                image.set(P.x, P.y, color);
            }
        }
    }
}