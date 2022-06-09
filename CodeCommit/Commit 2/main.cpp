#include <vector>
#include <iostream>
#include <limits>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include "rasterizer.h"

//定义颜色
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
//定义模型
Model *model = NULL;
//定义zbuffer
float *zbuffer = NULL;
//定义宽度，高度,深度
const int width = 800;
const int height = 800;
const int depth = 255;
//设置光照方向
Vec3f light_dir(0.2,0.15,-1);
//设置摄像机
Vec3f camera(0,0,3);
//世界坐标转屏幕坐标
Vec3f world2screen(Vec3f v)
{
    return Vec3f(int((v.x + 1.) * width / 2. + .5), int((v.y + 1.) * height / 2. + .5), v.z);
}
int main(int argc, char **argv)
{
        Rasterizer ras;
        if (2==argc) 
                model = new Model(argv[1]);  //命令行控制方式构造model
        else
                model = new Model("obj/african_head.obj"); //代码方式构造model

        //构造tga
        TGAImage image(width,height, TGAImage::RGB);
        zbuffer = image.get_zbuffer();
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
        //                 ras.line(x0,y0, x1,y1, image, white);
        //         }  
        // }

        //平面着色-----------------------------------------------------------------------------------

        //初始化投影矩阵为4x4的单位矩阵
        Matrix Projection = Matrix::identity(4);
        //投影矩阵[3][2] = -1/c,c为相机z坐标
        Projection[3][2] = -1.0f/camera.z;
        //初始化视角矩阵
        Matrix ViewPort = Matrix::viewport(width/8, height/8, width*3/4, height*3/4,depth);

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

                        screen_coords[j] = world2screen(model->vert(face[j]));//box，z轴用于表示zbuffer，没有变化
                        //视角矩阵*投影矩阵*坐标
                        // screen_coords[j] = Matrix::m2v(ViewPort * Projection * Matrix::v2m(v)); //line，三个顶点的坐标

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
                        // ras.triangle_line(screen_coords,uv, model, image, intensity, zbuffer);
                        ras.triangle_box(screen_coords,uv, model, image, intensity, zbuffer);
                }            
        }
        image.flip_vertically(); //翻转
        image.write_tga_file(".tgaFile/output.tga");
    {
        TGAImage zbimage(width, height, TGAImage::GRAYSCALE);
        for (int i=0; i<width; i++) {
            for (int j=0; j<height; j++) {
                zbimage.set(i, j, TGAColor(zbuffer[i+j*width]*650,1));
            }
        }
        zbimage.flip_vertically();
        zbimage.write_tga_file(".tgaFile/zbuffer.tga");
    }
    delete model;
    delete [] zbuffer;
    return 0;
}
