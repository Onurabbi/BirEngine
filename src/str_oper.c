#include <string.h>
#include <stdio.h>

const char* directorize(const char* path)
{
    char* res = (char*)malloc(128*sizeof(char));
    int i=0; int lastIndex = 0;
    while(path[i]!='\0')
    {
        res[i]=path[i];
        if(path[i]=='/')
            lastIndex = i;
        i++;
    }
   
    res = (char*)realloc(res, lastIndex+2);
    res[lastIndex+1]='\0';
    return res;
}

const char* combine(const char* item, const char* directory)
{
    char* filename = (char*)malloc(1024*sizeof(char));
    unsigned int len = strlen(item);
    len+=strlen(directory);
    strcpy(filename, directory);
    strcat(filename, item);
    filename = (char*)realloc(filename, len+1);
    return filename;
}

int main()
{

    const char* path = "assets/Medieval/Demonstration/vidivid/adasda/wqeqe/adsd/veli.amk";
    const char* directory = directorize(path);
    const char* file = "deneme.obj";
    const char* filename = combine(file, directory);
    printf("The file path is: %s\n", filename);
    getchar();
    return 0;
}