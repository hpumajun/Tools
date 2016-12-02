#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <assert.h>
#include "public/public.h"

#define COMMENTLINE 0
#define FUNCTIONDEFINE 1
#define IF			    2
#define WHILE		    3
#define SWITCH			4
#define LEFT			5
#define RIGHT			6
#define TEXT    	 	7

typedef struct content
{
	int line;
	char sz[80];
}

static int comment_line(char *szline)
{
	int flag = 0;
	char *p = szline;
	char c ;
//	printf("szline %d:%s",*szline_num,szline);
	while( *p != '\0')
	{
		c = *p;
		p++;
		// skip space charater
		if ( c == ' ')
			continue;
		else if ( c == '/' && *p++ == '*')
		{
			flag = 1;
			continue;
		}
		else if (c == '*' && *p++ == '/')
		{
			flag += 2;
			break;
		}
		else if (c = '/' && *p++ == '/')
		{
			flag = 4;
			break;
		}
		else
		{
			// if this is szline begin with /*, we should continue find the "*/", or we skip it and check new szline
			if(0 == flag)
				break;
			else
				continue;
		}
	}
	return flag;
}


int pase_line(char *szline)
{
	assert(szline);
	char *psz = szline;
	char c = *p;
	int index;
	if ((index = StringFind(psz,"if") > 0 )) 
	{
		return IF;
	}
	if ((index = StringFind(psz,"if") > 0
}

void jumpcommentline(FILE *fp,char * line, int *line_num)
{
	int type = 0;
	type = comment_line(line);
	switch(type)
	{
		case 1:
		{
			printf("comment begin from line %d\n",*line_num);
			while(fgets(line,256,fp))
			{
				printf("line %d:%s",*line_num,line);
				*line_num += 1;
				if(comment_line(line) == 2)
				{
					printf("comment end at line %d\n",*line_num);
					break;
				}
			}
			break;
		}
		case 3:
		{
			printf("line %d is signal up comment line\n",*line_num);
			break;
		}
		case 4:
		{
			printf("Line %d is sigle comment line\n", *line_num);
			break;
		}
		default:
			break;
	}
	return;
}

void findfunction(FILE * fp,int startline, int endline)
{
	char szbuffer[256] = {0};

}
void opensourcefile(char * path)
{
	FILE * fp;
	char line[256];
	int line_num = 1;
	fp = fopen(path,"r+");
	if (fp)
	{
		while(fgets(line,sizeof(line),fp))
		{
			printf("line %d,%s",line_num,line);
			jumpcommentline(fp,line,&line_num);
			line_num++;
		}
	}
	else
	{
		printf("open %s failed\n",path);
		printf("error: %s\n", strerror(errno));
	}
	fclose(fp);
}

void insertcontent(FILE *fp)
{
	f
}

void printchar()
{
	FILE * fp;
	char line[256];
	int line_num = 1;
	fp = fopen("b.txt","r+");
	fgets(line,sizeof(line),fp);
	int i; 
	for(i=0;line[i]!=0;i++)if(isspace(line[i])) 
		printf("line[%d]is a white-space character:%d\n",i,line[i]);
	fclose(fp);
}



int main(int argc,char *argv[])
{
//	opensourcefile("dhcp.c");
	printchar();
}
