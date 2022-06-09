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
//用画线法绘制三角形  顶点坐标，与之对应的uv坐标，TGA指针，亮度，zbuffer
void Rasterizer::triangle_line(Vec3f* pts, Vec2i *uv, Model *model, TGAImage &image, float intensity, float *zbuffer)
{
    Vec3i t0 = pts[0]; Vec3i t1 = pts[1]; Vec3i t2 = pts[2];
    Vec2i uv0 = uv[0]; Vec2i uv1 = uv[1]; Vec2i uv2 = uv[2];
    //排除三角形面积为0的情况
    if(t0.y == t1.y && t0.y == t2.y)
            return;
    //将三个点根据y的大小排序，使得t2.y>t1.y>t0.y, uv坐标也进行对应排序
    if(t0.y > t1.y) {std::swap(t0,t1); std::swap(uv0,uv1);}
    if(t0.y > t2.y) {std::swap(t0,t2); std::swap(uv0,uv2);}
    if(t1.y > t2.y) {std::swap(t1,t2); std::swap(uv1,uv2);}
    int total_height = t2.y - t0.y;
    //以高度差作为循环控制变量，使得每行都被填充
    for (int i = 0; i < total_height; i++)
    {
        //根据t1.y将三角形分为上下两部分
        bool second_half = (i > t1.y - t0.y || t1.y == t0.y);
        //当前段的高度
        int segment_height = second_half ? t2.y - t1.y : t1.y - t0.y;
        //alpha为i在整段中的占比
        float alpha = (float)i/total_height;
        //beta为i在当前段中的占比
        float beta = (float)(i-(second_half?t1.y - t0.y : 0))/segment_height;
        
        //计算A,B两点的坐标,A表示t0与t2之间的点，B表示t0与t1之间的点
        Vec3i A =               t0 + Vec3f(t2 - t0)*alpha;
        Vec3i B = second_half ? t1 + Vec3f(t2 - t1)*beta : t0 + Vec3f(t1 - t0)*beta;
        //计算uv
        Vec2i uvA =               uv0 + (uv2 - uv0)*alpha;
        Vec2i uvB = second_half ? uv1 + (uv2 - uv1)*beta : uv0 + (uv1 - uv0)*beta;
        //保证A在B的左边
        if(A.x > B.x) std::swap(A,B);
        //用横坐标作为循环控制，对这一行进行着色
        for(int j = A.x; j<= B.x; j++)
        {
            //计算当前点再AB之间的比例
            float phi = B.x == A.x ? 1.0 : (float)(j-A.x) / (float)(B.x - A.x);
            //计算出当前点的坐标，A、B保存了Z轴信息
            Vec3i P = Vec3f(A) + Vec3f(B-A)*phi;
            Vec2i uvP = uvA + (uvB - uvA)*phi;
            if(P.x < width && P.y < height)
            {
                //计算当前zbuffer的下标 = P.x+P.y*width
                 int idx = P.x+P.y*width;
                //当前点的z大于zbuffer信息，覆盖掉，并更新zbuffer
                if(zbuffer[idx] < P.z)
                {
                    zbuffer[idx] = P.z;
                    TGAColor color = model->diffuse(uvP);
                    image.set(P.x, P.y, TGAColor(color.r*intensity, color.g*intensity, color.b*intensity));
                }
            }
        }
    }
        
}
//计算质心坐标
Vec3f Rasterizer::barycentric(Vec3f A, Vec3f B,Vec3f C, Vec3f P)
{
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
void Rasterizer::triangle_box(Vec3f *pts, Vec2i *uv, Model *model, TGAImage &image, float intensity, float *zbuffer)
{
    Vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
    Vec2f clamp(image.get_width() - 1, image.get_height() - 1);
    //确定三角形的边框
    for (int i = 0; i < 3; i++)
    {
        //确定x轴边框
        bboxmin.x = std::max(0.0f, std::min(bboxmin.x, pts[i].x));
        bboxmax.x = std::min(clamp.x, std::max(bboxmax.x, pts[i].x));
        //确定y轴边框
        bboxmin.y = std::max(0.0f, std::min(bboxmin.y, pts[i].y));
        bboxmax.y = std::min(clamp.y, std::max(bboxmax.y, pts[i].y));
    }
    Vec3f P;
    Vec2i uvP;
    //遍历边框中的每一个点
    for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++)
    {
        for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++)
        {
            //防止越界
            if (P.x > 600 && P.y > 500)
                P.x += 0.01;
            //质心坐标
            Vec3f bc_screen = barycentric(pts[0], pts[1], pts[2], P);
            //如果质心坐标有一个负值，说明点在三角形外
            if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0)
                continue;
            //计算P点的颜色
            uvP = uv[0] * bc_screen.x + uv[1] * bc_screen.y + uv[2] * bc_screen.z;
            //计算zbuffer，并且每个顶点的z值乘上对应的质心坐标分量
            P.z = pts[0].z * bc_screen.x + pts[1].z * bc_screen.y + pts[2].z * bc_screen.z;
            //计算当前zbuffer的下标 = P.x+P.y*width
            int idx = P.x + P.y * width;
            if (zbuffer[idx] < P.z) //查找该像素在zubuffer上的位置，如果更靠近屏幕就渲染
            {
                zbuffer[idx] = P.z;
                TGAColor color = model->diffuse(uvP);
                image.set(P.x, P.y, TGAColor(color.r * intensity, color.g * intensity, color.b * intensity));
            }
        }
    }
}