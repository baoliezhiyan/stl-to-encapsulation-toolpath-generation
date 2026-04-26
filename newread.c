#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct Head Head;
struct Head
{
	char partName[80];
	int  faceNum;
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

double sqrt_iterative(double num) {
    if (num < 0) return -1;  // 对于负数，我们定义这个函数返回-1
    double error = 1e-7; // 定义误差界限
    double t = num;
    while (t - num / t > error) {
        t = (t + num / t) / 2.0;
    }
    return t;
}

int main()
{
	Head head;
	char fileName[128];
 
	FILE* fp= fopen("readfile.STL","rb"); 
 
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

	FILE* out= fopen("out.txt","w"); 
	/*fprintf(out,"STL文件解析如下：\n");
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
	}*/
	float x1,x2,x3,y1,y2,y3,mx1,mx2,my1,my2,k1,k2;
	float one=1;
	float zero=0;
	int count=0;

	for (int i = 0; i < head.faceNum; i++)
	{
		if ((faces[i].normal.j==zero)&&(faces[i].normal.i!=one)&&(faces[i].normal.k!=one))
		{
			count++;
			if (count==1)
			{
				x1=faces[i].p1.x;
				y1=faces[i].p1.z;
				if ((faces[i].p2.x!=x1)||(faces[i].p2.z!=y1))
				{
					x2=faces[i].p2.x;
					y2=faces[i].p2.z;
				}
				if ((faces[i].p3.x!=x1)||(faces[i].p3.z!=y1))
				{
					x2=faces[i].p3.x;
					y2=faces[i].p3.z;
				}	
			} else if (count==2)
			{
				if (((faces[i].p1.x!=x1)||(faces[i].p1.z!=y1))&&((faces[i].p1.x!=x2)||(faces[i].p1.z!=y2)))
				{
					x3=faces[i].p1.x;
					y3=faces[i].p1.z;
				}
				if (((faces[i].p2.x!=x1)||(faces[i].p2.z!=y1))&&((faces[i].p2.x!=x2)||(faces[i].p2.z!=y2)))
				{
					x3=faces[i].p2.x;
					y3=faces[i].p2.z;
				}
				if (((faces[i].p3.x!=x1)||(faces[i].p3.z!=y1))&&((faces[i].p3.x!=x2)||(faces[i].p3.z!=y2)))
				{
					x3=faces[i].p3.x;
					y3=faces[i].p3.z;
				}
			} else if (count>2)
			{
				break;
			}
		}
	}
	fprintf(out,"%f  %f\n",x1,y1);
	fprintf(out,"%f  %f\n",x2,y2);
	fprintf(out,"%f  %f\n",x3,y3);
	mx1=(x1+x2)/2;
	my1=(y1+y2)/2;
	mx2=(x2+x3)/2;
	my2=(y2+y3)/2;
	k1=(-1)/((y2-y1)/(x2-x1));
	k2=(-1)/((y3-y2)/(x3-x2));
	fprintf(out,"%f  %f\n",mx1,my1);
	fprintf(out,"%f  %f\n",mx2,my2);
	fprintf(out,"%f  %f\n",k1,k2);
	float xcenter=(my2-my1+(k1*mx1)-(k2*mx2))/(k1-k2);
	float ycenter=(k1*xcenter)+(my1-(k1*mx1));
	fprintf(out,"%f  %f\n",xcenter,ycenter);
	double radius=(x1-xcenter)*(x1-xcenter)+(y1-ycenter)*(y1-ycenter);
	fprintf(out,"%f \n",radius);
	double result = sqrt_iterative(radius);
	fprintf(out,"%f \n",result);
	return 0;
}