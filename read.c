#include<stdio.h>
#include<stdlib.h>
#include<string.h>
 
typedef struct Head Head;
struct Head
{
    char partName[80];
    int faceNum;
};

typedef struct Point Point;
struct Point
{
	float x;
	float y;
	float z;
};

typedef struct Normal Normal;
struct Normal
{
	float i;
	float j;
	float k;
};

typedef struct Face Face;
struct Face
{
	Normal normal;
	Point  p1;
	Point  p2;
	Point  p3;
	char  info[2];
};

typedef struct readchip readchip;
struct readchip
{
    float min_x;
    float max_x;
    float min_y;
    float max_y;
    float height;
};

typedef struct chip chip;
struct chip
{
    int min_x;
    int max_x;
    int min_y;
    int max_y;
    int height;
};

typedef struct node node;
struct node
{
    int x;
    int y;
    int node_count;
    node* next_node;
};
 
int main()
{
	Head head;
	FILE *fp;
	char fileName[128];
 

	fp = fopen("source.STL","rb");
 
	if (fp != NULL)
	{
		fread(head.partName, 80, 1, fp);//获取部件名
		fread(&head.faceNum, 4, 1, fp);//获取三角面片的数目
	}
 
	Face *faces = (Face*)malloc(head.faceNum * sizeof(Face));//根据三角面片的数目，创建一个Face类型的数组
 
	//循环读取三角面片数据
	for (int i = 0;i < head.faceNum;i++)
	{
		fread(&faces[i].normal, 12, 1, fp);//读取法线数据
		fread(&faces[i].p1, 12, 1, fp);//读取顶点1的数据
		fread(&faces[i].p2, 12, 1, fp);//读取顶点2的数据
		fread(&faces[i].p3, 12, 1, fp);//读取顶点3的数据
		fread(&faces[i].info,2, 1, fp);//读取保留项数据，这一项一般没什么用，这里选择读取是为了移动文件指针
	}
 
	fclose(fp);
	FILE *out;
	out = fopen("out.txt","w");
 
	fprintf(out,"STL文件解析如下：\n");
	fprintf(out,"三角形数目：");
	fprintf(out,"%d\n", head.faceNum);
	fprintf(out,"各面的顶点和法线数据如下：\n");

 
	//在控制台上输出STL文件的数据
	for (int i = 0;i < head.faceNum;i++)
	{
		fprintf(out,"面%d\n", i + 1);
		fprintf(out,"法线：\n");
		fprintf(out,"%f  %f  %f\n", faces[i].normal.i, faces[i].normal.j, faces[i].normal.k);
		fprintf(out,"顶点数据：\n");
		fprintf(out,"%f  %f  %f\n", faces[i].p1.x, faces[i].p1.y, faces[i].p1.z);
		fprintf(out,"%f  %f  %f\n", faces[i].p2.x, faces[i].p2.y, faces[i].p2.z);
		fprintf(out,"%f  %f  %f\n", faces[i].p3.x, faces[i].p3.y, faces[i].p3.z);
	}
 
	return 0;
}