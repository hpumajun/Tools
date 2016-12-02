#include <stdio.h>
#include <ctype.h>

struct mystruct
{
    int i;
    char cha[80];
};
 
int test(void)
{
    FILE *stream;
	char sz[80];
    if ((stream = fopen("TEST.c", "w+")) == NULL) /* open file TEST.$$$ */
    {
        fprintf(stderr, "Cannot open output file.\n");
        
        return 1;
    }
//    fgets(sz,sizeof(sz),stream);
 //   printf("%s\n",sz);
    char s[80] = "/*************************************/\n";
    fwrite(&s, sizeof(s), 1, stream); /* 写的struct文件*/
    fwrite(&s, sizeof(s), 1, stream);
    fclose(stream); /*关闭文件*/
    return 0;
}
int StringFind(const char *pSrc, const char *pDst)  
{  
    int i, j;  
    for (i=0; pSrc[i]!='\0'; i++)  
    {  
        if(pSrc[i]!=pDst[0])  
            continue;         
        j = 0;  
        while(pDst[j]!='\0' && pSrc[i+j]!='\0')  
        {  
            j++;  
            if(pDst[j]!=pSrc[i+j])  
            break;  
        }  
        if(pDst[j]=='\0')  
            return i;  
    }  
    return -1;  
} 



int main(int argc, char * argv[])
{  
	/*
	char t[100] = "do_extract (struct dhcp *dhcp, const int optlen)";
	printf("%d\n",StringFind(t,"struct"));
	* */
	test();
//　　getchar();  

} 
