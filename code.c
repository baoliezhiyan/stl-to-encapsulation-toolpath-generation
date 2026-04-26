#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define initial_x 25.5
#define initial_y 27.0
#define initial_z 14.0  // in test is 17
#define unit_x 0.5
#define unit_y 0.5
#define unit_z 1.0
#define speed 3500
#define quick_speed 8000
#define xbasemodify -1.0
#define ybasemodify 0.0
#define xchipmodify -1.0
#define ychipmodify 0.0

float xwidth=0;
float ylength=0;
float zheight=0;
int chip_number=0;

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

int 
search(FILE* file,node* current_node,node* startnode,int* arr,int node_number,int x_scale, int y_scale){
    *(arr+y_scale*current_node->x+current_node->y)=-1;
    if (current_node->node_count==node_number)
    {
        current_node->next_node=NULL;
        int x_count=0;
        int y_count=0;
        while (startnode->node_count<node_number)
        {
            int x_differ=startnode->next_node->x-startnode->x;
            int y_differ=startnode->next_node->y-startnode->y;
            if (x_differ!=0)
            {
                x_count=x_count+x_differ;
                if (y_count!=0)
                {
                    fprintf(file,"G0 Y%.1f\n",unit_y*y_count);
                    y_count=0;
                }
            } 
            if (y_differ!=0)
            {
                y_count=y_count+y_differ;
                if (x_count!=0)
                {
                    fprintf(file,"G0 X%.1f\n",unit_x*x_count);
                    x_count=0;
                }    
            }
            if (startnode->node_count==node_number-1)
            {
                fprintf(file,"G0 X%.1f Y%.1f\n",unit_x*x_count,unit_y*y_count);
                return -1;
            }
            startnode=startnode->next_node;
        }
    }
    if ((current_node->x-1)>=0)
    {
        if (*(arr+y_scale*current_node->x+current_node->y-y_scale)==0)
        {
            node* next_node=(node*)malloc(sizeof(node));
            next_node->x=current_node->x-1;
            next_node->y=current_node->y;
            next_node->node_count=current_node->node_count+1;
            current_node->next_node=next_node;
            if (-1==search(file,next_node,startnode,arr,node_number,x_scale,y_scale))
            {
                return -1;
            }
            free(next_node);
        }    
    }
    if ((current_node->y+1)<y_scale)
    {
        if (*(arr+y_scale*current_node->x+current_node->y+1)==0)
        {
            node* next_node=(node*)malloc(sizeof(node));
            next_node->x=current_node->x;
            next_node->y=current_node->y+1;
            next_node->node_count=current_node->node_count+1;
            current_node->next_node=next_node;
            if (-1==search(file,next_node,startnode,arr,node_number,x_scale,y_scale))
            {
                return -1;
            }
            free(next_node);
        }    
    }
    if ((current_node->y-1)>=0)
    {
        if (*(arr+y_scale*current_node->x+current_node->y-1)==0)
        {
            node* next_node=(node*)malloc(sizeof(node));
            next_node->x=current_node->x;
            next_node->y=current_node->y-1;
            next_node->node_count=current_node->node_count+1;
            current_node->next_node=next_node;
            if (-1==search(file,next_node,startnode,arr,node_number,x_scale,y_scale))
            {
                return -1;
            }
            free(next_node);
        }    
    }
    if ((current_node->x+1)<x_scale)
    {
        if (*(arr+y_scale*current_node->x+current_node->y+y_scale)==0)
        {
            node* next_node=(node*)malloc(sizeof(node));
            next_node->x=current_node->x+1;
            next_node->y=current_node->y;
            next_node->node_count=current_node->node_count+1;
            current_node->next_node=next_node;
            if (-1==search(file,next_node,startnode,arr,node_number,x_scale,y_scale))
            {
                return -1;
            }
            free(next_node);
        }    
    }
    *(arr+y_scale*current_node->x+current_node->y)=0;
}

void
path(FILE* file,int x_start,int x_end,int y_start,int y_end,int height){
    int x_range=x_end-x_start;
    int y_range=y_end-y_start;
    int array[x_range][y_range];
    for (int i = 0; i < x_range; i++)
    {
        for (int j = 0; j < y_range; j++)
        {
            array[i][j]=0;
        }
    }
    int node_number=x_range*y_range;
    node* startnode=(node*)malloc(sizeof(node));
    startnode->x=0;
    startnode->y=0;
    startnode->node_count=1;
    fprintf(file,"G90\n");
    fprintf(file,"G0 X%.1f Y%.1f Z%.1f F%d\n",initial_x+unit_x/2+unit_x*x_start,initial_y+unit_y/2+unit_y*y_start,initial_z+zheight+unit_z*height,quick_speed);
    fprintf(file,"G91\n");
    fprintf(file,"G0 F%d\n",speed); // set speed
    search(file,startnode,startnode,*array,node_number,x_range,y_range);
}

void 
chip_path(FILE* file,int x_start,int x_end,int y_start,int y_end,int height){
    fprintf(file,"G90\n");
    fprintf(file,"G0 X%.1f Y%.1f Z%.1f F%d\n",initial_x+unit_x/2+unit_x*x_start,initial_y+unit_y/2+unit_y*(y_start-1),initial_z+zheight+unit_z,quick_speed);
    fprintf(file,"G91\n");
    fprintf(file,"G0 F%d\n",quick_speed); // set speed
    for (int i = 1; i <= height; i++)
    {
        fprintf(file,"G0 X%.1f\n",-unit_x);
        fprintf(file,"G0 F%d\n",speed); // set speed
        fprintf(file,"G0 Y%.1f\n",(y_end-y_start+1)*unit_y);
        fprintf(file,"G0 X%.1f\n",(x_end-x_start+1)*unit_x);
        fprintf(file,"G0 Y%.1f\n",-(y_end-y_start+1)*unit_y);
        fprintf(file,"G0 X%.1f\n",(x_start-x_end)*unit_x);
        fprintf(file,"G0 F%d\n",quick_speed); // set speed
        fprintf(file,"G0 Z%.1f\n",unit_z);
    }
    path(file,x_start,x_end,y_start,y_end,height);
    path(file,x_start,x_end,y_start,y_end,height+1);
}

int main(){
    // read
    // skip the head
	Head head;
	FILE* fp= fopen("source.STL","rb"); 
	if (fp != NULL)
	{
		fread(head.partName, 80, 1, fp);
		fread(&head.faceNum, 4, 1, fp);
	}
	// read all the faces
	Face *faces = (Face*)malloc(head.faceNum * sizeof(Face));
	for (int i = 0;i < head.faceNum;i++)
	{
		fread(&faces[i].normal, 12, 1, fp);
		fread(&faces[i].p1, 12, 1, fp);
		fread(&faces[i].p2, 12, 1, fp);
		fread(&faces[i].p3, 12, 1, fp);
		fread(&faces[i].info,2, 1, fp);
	}
	fclose(fp);
	// read the data of the base
	for (int i = 0; i < head.faceNum; i++)
	{
		if (faces[i].normal.k==-1)
		{
			if (faces[i].p1.x>=xwidth)
			{
				xwidth=faces[i].p1.x;
			}
			if (faces[i].p2.x>=xwidth)
			{
				xwidth=faces[i].p2.x;
			}
			if (faces[i].p3.x>=xwidth)
			{
				xwidth=faces[i].p3.x;
			}
			if (faces[i].p1.y>=ylength)
			{
				ylength=faces[i].p1.y;
			}
			if (faces[i].p2.y>=ylength)
			{
				ylength=faces[i].p2.y;
			}
			if (faces[i].p3.y>=ylength)
			{
				ylength=faces[i].p3.y;
			}
		}
		if (faces[i].normal.k==1)
		{
			if (zheight==0)
			{
				zheight=faces[i].p1.z;
			} else if (zheight>faces[i].p1.z)
			{
				zheight=faces[i].p1.z;
			}
		}
	}
    // read the number of chips
	int chip_number=0;
	for (int i = 0; i < head.faceNum; i++)
	{
		if ((faces[i].normal.k==1)&&(faces[i].p1.z!=zheight))
		{
			chip_number++;
		}
	}
	chip_number=chip_number/2;
    // read the data of chips
	readchip* readchips = (readchip*)malloc(chip_number*sizeof(readchip));
	int currentcount=0;
	for (int i = 0; i < head.faceNum; i++)
	{
		if ((faces[i].normal.k==1)&&(faces[i].p1.z!=zheight))
		{
			readchip* tempchip = (readchip*)malloc(sizeof(readchip));
			tempchip->height=faces[i].p1.z;
			tempchip->min_x=faces[i].p1.x;
			tempchip->max_x=faces[i].p2.x;
			if (tempchip->min_x>tempchip->max_x)
			{
				int temp;
				temp=tempchip->max_x;
				tempchip->max_x=tempchip->min_x;
				tempchip->min_x=temp;
			}
			if (faces[i].p3.x<tempchip->min_x)
			{
				tempchip->min_x=faces[i].p3.x;
			}
			if (faces[i].p3.x>tempchip->max_x)
			{
				tempchip->max_x=faces[i].p3.x;
			}
			tempchip->min_y=faces[i].p1.y;
			tempchip->max_y=faces[i].p2.y;
			if (tempchip->min_y>tempchip->max_y)
			{
				int temp;
				temp=tempchip->max_y;
				tempchip->max_y=tempchip->min_y;
				tempchip->min_y=temp;
			}
			if (faces[i].p3.y<tempchip->min_y)
			{
				tempchip->min_y=faces[i].p3.y;
			}
			if (faces[i].p3.y>tempchip->max_y)
			{
				tempchip->max_y=faces[i].p3.y;
			}
			int new=1;
			for (int j = 0; j < currentcount; j++)
			{
				if ((readchips[j].height==tempchip->height)&&(readchips[j].min_x==tempchip->min_x)&&(readchips[j].max_x==tempchip->max_x)&&(readchips[j].min_y==tempchip->min_y)&&(readchips[j].max_y==tempchip->max_y))
				{
					new=0;
				}
			}
			if (new==1)
			{
				readchips[currentcount].height=tempchip->height;
				readchips[currentcount].min_x=tempchip->min_x;
				readchips[currentcount].max_x=tempchip->max_x;
				readchips[currentcount].min_y=tempchip->min_y;
				readchips[currentcount].max_y=tempchip->max_y;
				currentcount++;
			}
			free(tempchip);
		}
	}
	// modify the data
	xwidth=(int)(xwidth*10.)/10.;
    xwidth=xwidth+xbasemodify;
	ylength=(int)(ylength*10.)/10.;
    ylength=ylength+ybasemodify;
	zheight=(int)(zheight*10.)/10.;
	for (int i = 0; i < chip_number; i++)
	{
		readchips[i].height=(int)(readchips[i].height*10.)/10;
		readchips[i].height=readchips[i].height-zheight;
		readchips[i].min_x=(int)(readchips[i].min_x*10.)/10;
        readchips[i].min_x=readchips[i].min_x+xchipmodify;
		readchips[i].max_x=(int)(readchips[i].max_x*10.)/10;
        readchips[i].max_x=readchips[i].max_x+xchipmodify;
		readchips[i].min_y=(int)(readchips[i].min_y*10.)/10;
		readchips[i].max_y=(int)(readchips[i].max_y*10.)/10;
	}

    // print
    FILE* file = fopen("code.gcode","w");
    if (file == NULL)
    {
        printf("error");
        return 1;
    }
    fprintf(file,"M107\n");
    // base
    int x_range=xwidth/unit_x;
    int y_range=ylength/unit_y;
    int array[x_range][y_range];
    chip* chips = (chip*)malloc(chip_number*sizeof(chip));
    for (int i = 0; i < chip_number; i++)
    {
        chips[i].height=readchips[i].height/unit_z;
        chips[i].min_x=readchips[i].min_x/unit_x;
        chips[i].max_x=readchips[i].max_x/unit_x;
        chips[i].min_y=readchips[i].min_y/unit_y;
        chips[i].max_y=readchips[i].max_y/unit_y;
    }
    for (int i = 0; i < x_range; i++)
    {
        for (int j = 0; j < y_range; j++)
        {
           array[i][j]=0;
        }
    }
    int node_number=x_range*y_range;
    for (int k = 0; k < chip_number; k++)
    {
        for (int i = chips[k].min_x-1; i < chips[k].max_x+1; i++)
        {
            for (int j = chips[k].min_y-1; j < chips[k].max_y+1; j++)
            {
                array[i][j]=1;
            }
        }
        node_number=node_number-(chips[k].max_x-chips[k].min_x+2)*(chips[k].max_y-chips[k].min_y+2);
    }
    node* startnode=(node*)malloc(sizeof(node));
    startnode->x=0;
    startnode->y=0;
    startnode->node_count=1;
    fprintf(file,"G90\n");
    fprintf(file,"G0 X%.1f Y%.1f Z%.1f F%d\n",initial_x+unit_x/2,initial_y+unit_y/2,initial_z+zheight,quick_speed);
    fprintf(file,"G91\n");
    fprintf(file,"G0 F%d\n",speed); //set speed
    search(file,startnode,startnode,*array,node_number,x_range,y_range);
    // print chips
    for (int i = 0; i < chip_number; i++)
    {
        chip_path(file,chips[i].min_x,chips[i].max_x,chips[i].min_y,chips[i].max_y,chips[i].height);
    }
    //end
    fprintf(file,"M107\n");
    fprintf(file,"M104 T0 S0\n");
    fprintf(file,"M140 S0\n");
    fprintf(file,"M84\n");
    fclose(file);
    return 0;
}