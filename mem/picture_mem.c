#include <stdio.h>
#include <stdlib.h>
#include <linux/types.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>


unsigned long get_picture_size(const char *path)  
{  
    unsigned long filesize = -1;      
    struct stat statbuff;  
    if(stat(path, &statbuff) < 0){  
        return filesize;  
    }else{  
        filesize = statbuff.st_size;  
    }  
    return filesize;  
}  


int main(int argc,char *argv[])
{
    char *file_name=argv[1];
    int fd=open(file_name,O_RDWR);
    
    
    int picture_size = get_picture_size(file_name);

    uint8_t *pImg=malloc(sizeof(uint8_t)*picture_size);

    if(read(fd,pImg,picture_size)<0)
    {
        printf("error!\n");
    }
    close(fd);

    int index=1024;
    for(;index<2000;index++)
    {
        *(pImg+index)=255;
    }    
    fd=open("2.jpg",O_RDWR | O_CREAT | O_TRUNC);

    write(fd,pImg,picture_size);
    close(fd);

    return 1;
}
