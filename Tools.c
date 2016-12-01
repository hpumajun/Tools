#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#define COMMENTLINE 0
#define FUNCTIONDEFINE 1
#define IF			    2
#define WHILE		    3
#define SWITCH			4
#define LEFT
#define RIGHT
static int comment_line(char *line)
{
	int flag = 0;
	while(line)
	{
		if (*line == ' ')
			continue;
		if (*line == '/' && *(++line) == '*')
			flag = 1;
			break;
		else (*line == '*' && *(++line) == '/')
			flag = 2;
	}
	return flag;
	
}


int pase_line(char *line, int line_num)
{
	if(line)
	{
		if (iscommentline(line))
			printf ("Line %d is comment line",line_num);
			return COMMENTLINE;
		if ()
	}
	
}

void jumpcommentline(FILE *fp,char * line, int line_num)
{
	if (comment_line(line) == 1 )
	{
		printf("comment begin %d\n",line_num);
		fgets(line,sizeof(line),fp);
		line_num ++;
	}
	else if (comment_line(line) == 2)
	{
		printf("comment end %d\n",line_num);
		return;
	}
	else
		jumpcommentline(fp,line,line_num)
	
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
			printf("line %d,%s",line_num++,line);
			jumpcommentline(fp,line,line_num);
		}
	}
	else
	{
		printf("open %s failed\n",path);
		printf("error: %s\n", strerror(errno));
	}
}

int main(int argc,char *argv[])
{
	opensourcefile("dhcp.c");
}
