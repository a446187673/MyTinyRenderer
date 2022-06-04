#include <vector>
#include <iostream>
#include <limits>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
using namespace std;
//定义颜色
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
//定义模型
Model *model = NULL;
//定义zbuffer
int *zbuffer = NULL;
//定义宽度，高度,深度
const int width = 800;
const int height = 800;
const int depth = 255;
//设置光照方向
Vec3f light_dir(0.2,0.15,-1);
//设置摄像机
Vec3f camera(0,0,3);

//4d->3d
//除以最后一个分量，当最后一个分量为0，表示向量。不为0，表示坐标
Vec3f m2v(Matrix m)
{
        return Vec3f(m[0][0]/m[3][0], m[1][0]/m[3][0], m[2][0]/m[3][0]);
}
//3d->4d
//添加一个1表示坐标
Matrix v2m(Vec3f v)
{
        Matrix m(4,1);
        m[0][0] = v.x;
        m[1][0] = v.y;
        m[2][0] = v.z;
        m[3][0] = 1.0f;
        return m;
}

//视角矩阵
//将物体x，y坐标(-1,1)转到屏幕坐标(100,700) 1/8width ~ 7/8width
//zbuffer(-1,1)转换到0~255
Matrix viewport(int x, int y, int w, int h)
{
        //创建一个4x4单位矩阵m
        Matrix m = Matrix::identity(4);
        //第四列表示平移信息
        m[0][3] = x + w/2.0f;
        m[1][3] = y + h/2.0f;
        m[2][3] = depth/2.0f;
        //对角线表示缩放信息
        m[0][0] = w/2.0f;
        m[1][1] = h/2.0f;
        m[2][2] = depth/2.0f;
        return m;
}
void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
{
        bool steep = false; //steep表示斜率是否大于1，false为斜率小于1，即Δx>Δy
        if(std::abs(x0 - x1) < std::abs(y0 - y1))
        {       //斜率大于1，steep变为true，交换x0，y0，防止线段不连续
                std::swap(x0,y0);
                std::swap(x1,y1);
                steep = true;
        }
        if (x0 > x1) //保证x1是更大的数，x0是更小的数
        {
                std::swap(x0, x1);
                std::swap(y0, y1);
        }
        int dx = x1 - x0;
        int dy = y1 - y0;
        int derror = std::abs(dy)*2; //可以理解为与斜率等于1时的误差
        int error = 0;
        int y = y0;
        for (int x = x0; x <= x1; x++)
        {
                if(steep == false)
                        image.set(x, y, color);
                else
                        image.set(y, x, color);
                error += derror;
                if(error > dx)  //当误差大于一定程度时y+1或-1
                {
                        y += y1>y0 ? 1:-1;
                        error -= dx*2;
                }
        }
}
//绘制三角形旧方法  顶点坐标，与之对应的uv坐标，TGA指针，亮度，zbuffer
void old_triangle(Vec3i t0, Vec3i t1, Vec3i t2, Vec2i uv0, Vec2i uv1, Vec2i uv2, TGAImage &image, float intensity, int *zbuffer)
{
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
Vec3f barycentric(Vec3f A, Vec3f B,Vec3f C, Vec3f P)
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
//绘制三角形(坐标数组，zbuffer指针，tga指针，颜色)
void triangle(Vec3f *pts, float *zbuffer, TGAImage &image, TGAColor color) 
{
        Vec2f bboxmin( std::numeric_limits<float>::max(),  std::numeric_limits<float>::max());
        Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
        Vec2f clamp(image.get_width() - 1, image.get_height() - 1);
        //确定三角形的边框
        for (int i = 0; i < 3; i++)
        {
                //确定x轴边框
                bboxmin.x = std::max(0.0f, std::min(bboxmin.x,pts[i].x));
                bboxmax.x = std::min(clamp.x,std::max(bboxmax.x,pts[i].x));
                //确定y轴边框
                bboxmin.y = std::max(0.0f, std::min(bboxmin.y,pts[i].y));
                bboxmax.y = std::min(clamp.y,std::max(bboxmax.y,pts[i].y));
        }
        Vec3f P;
        //遍历边框中的每一个点
        for(P.x = bboxmin.x; P.x <= bboxmax.x; P.x++)
        {
                for(P.y = bboxmin.y; P.y <= bboxmax.y; P.y++)
                {
                        //防止越界
                        if(P.x > 600 && P.y > 500)
                                P.x += 0.01;
                        //质心坐标
                        Vec3f bc_screen = barycentric(pts[0], pts[1],pts[2],P);
                        //如果质心坐标有一个负值，说明点在三角形外
                        if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0) 
                                continue;
                        //计算zbuffer，并且每个顶点的z值乘上对应的质心坐标分量
                        P.z = pts[0].z*bc_screen.x + pts[1].z*bc_screen.y + pts[2].z*bc_screen.z;
                        if(zbuffer[int(P.x + P.y*width)] < P.z) //查找该像素在zubuffer上的位置，如果更靠近屏幕就渲染
                        {
                                zbuffer[int(P.x + P.y*width)] = P.z;
                                image.set(P.x,P.y,color);
                        }
                        
                }
        }
}
//世界坐标转屏幕坐标
Vec3f world2screen(Vec3f v) {
    return Vec3f(int((v.x+1.)*width/2.+.5), int((v.y+1.)*height/2.+.5), v.z);
}
int main(int argc, char **argv)
{
        if (2==argc) 
                model = new Model(argv[1]);  //命令行控制方式构造model
        else
                model = new Model("obj/african_head.obj"); //代码方式构造model

        //构造tga
        TGAImage image(width,height, TGAImage::RGB);
        //构造zbuffer
        zbuffer = new int[width*height];
        for (int i=0; i<width*height; i++)
        //初始化zbuffer
        zbuffer[i] = std::numeric_limits<int>::min();


        //线框绘制---------------------------------------------------------------------------------
        // for (int i = 0; i < model->nfaces(); i++) //遍历所有三角形面
        // {
        //         std::vector<int> face = model->face(i); //当前面，储存3个顶点Vec3f
        //         for (int j = 0; j < 3; j++)
        //         {       //寻找当前面上的两个相邻顶点
        //                 Vec3f v0 = model->vert(face[j]); //顶点v0
        //                 Vec3f v1 = model->vert(face[(j+1)%3]); //顶点v1
        //                 //连接两相邻顶点
        //                 //先要进行模型坐标到屏幕坐标的转换
        //                 int x0 = (v0.x+1.0)*width/2.0;  //映射公式,世界坐标转平面坐标
        //                 int y0 = (v0.y+1.0)*height/2.0;
        //                 int x1 = (v1.x+1.0)*width/2.0;
        //                 int y1 = (v1.y+1.0)*height/2.0;
        //                 //画线
        //                 line(x0,y0, x1,y1, image, white);
        //         }  
        // }

        //平面着色-----------------------------------------------------------------------------------

        //初始化投影矩阵为4x4的单位矩阵
        Matrix Projection = Matrix::identity(4);
        //投影矩阵[3][2] = -1/c,c为相机z坐标
        Projection[3][2] = -1.0f/camera.z;
        //初始化视角矩阵
        Matrix ViewPort = viewport(width/8, height/8, width*3/4, height*3/4);

        //遍历所有三角形面
        for (int i = 0; i < model->nfaces(); i++)
        {
                //当前面，储存3个顶点Vec3f
                std::vector<int> face = model->face(i);
                //数组，用于储存三个顶点的屏幕坐标
                Vec3f screen_coords[3];
                //数组，用于储存三个顶点的世界坐标
                Vec3f world_coords[3];
                for (int j = 0; j < 3; j++)
                {
                        //取出一个顶点
                        Vec3f v = model->vert(face[j]);
                        //视角矩阵*投影矩阵*坐标
                        screen_coords[j] = m2v(ViewPort * Projection * v2m(v));
                        world_coords[j] = v; //世界坐标
                }
                //用世界坐标计算当前面的法向量
                Vec3f n = (world_coords[2] - world_coords[0])^(world_coords[1] - world_coords[0]);
                //归一化
                n.normalize(); 
                //光照强度=法向量*光照方向   即法向量和光照方向重合时，亮度最高
                float intensity = n*light_dir;
                intensity = std::min(std::abs(intensity),1.0f);
                //强度小于0，说明平面朝向为内  即背面裁剪
                if (intensity>0) 
                {
                        Vec2i uv[3];
                        //取出uv数组
                        for (int k = 0; k < 3; k++)
                                uv[k] = model->uv(i,k);
                        //绘制三角形
                        old_triangle(screen_coords[0], screen_coords[1], screen_coords[2], uv[0], uv[1], uv[2], image, intensity, zbuffer);
                }
        }
        image.flip_vertically(); //翻转
        image.write_tga_file(".tgaFile/output.tga");
        //输出zbuffer
        {
        TGAImage zbimage(width, height, TGAImage::GRAYSCALE);
        for (int i=0; i<width; i++) {
            for (int j=0; j<height; j++) {
                zbimage.set(i, j, TGAColor(zbuffer[i+j*width], 1));
            }
        }
        zbimage.flip_vertically();
        zbimage.write_tga_file(".tgaFile/zbuffer.tga");
        }
        delete model;
        return 0;
}
