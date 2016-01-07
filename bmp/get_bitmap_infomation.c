#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned long LONG;

typedef struct tagBITMAPFILEHEADER{
    WORD bftype;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
}BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER{
    DWORD biSize;
    DWORD biWidth;
    DWORD biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    DWORD biXPelsPerMeter;
    DWORD biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
}BITMAPINFOHEADER;

typedef struct tagRGBQUAD{
    BYTE rgbBlue;
    BYTE rgbGreen;
    BYTE rgbRed;
    BYTE rgbReserved;
}RGBQUAD;


void show_bitmap_head(BITMAPFILEHEADER *pFilehead)
{
    printf("*************位图文件头**************\n");
    printf("文件类型：%d\n",pFilehead->bftype);
    printf("文件大小：%d\n",pFilehead->bfSize);
    printf("保留字段1：%d\n",pFilehead->bfReserved1);
    printf("保留字段2：%d\n",pFilehead->bfReserved2);
    printf("偏移量：%d\n",pFilehead->bfOffBits);
}


void show_bitmap_headinfo(BITMAPINFOHEADER *pInfohead)
{
    printf("************位图信息头***************\n");
    printf("结构体的长度:%d\n",pInfohead->biSize);
    printf("位图宽:%d\n",pInfohead->biWidth);
    printf("位图高:%d\n",pInfohead->biHeight);
    printf("biPlanes平面数:%d\n",pInfohead->biPlanes);
    printf("biBitCount采用颜色位数:%d\n",pInfohead->biBitCount);
    printf("压缩方式:%d\n",pInfohead->biCompression);
    printf("biSizeImage实际位图数据占用的字节数:%d\n",pInfohead->biSizeImage);
    printf("X方向分辨率:%d\n",pInfohead->biXPelsPerMeter);
    printf("Y方向分辨率:%d\n",pInfohead->biYPelsPerMeter);
    printf("使用的颜色数:%d\n",pInfohead->biClrUsed);
    printf("重要颜色数:%d\n",pInfohead->biClrImportant);
}

int main(int argc,char *argv[])
{
    int fd_bmp;

    if((fd_bmp=open(argv[1],O_RDONLY)) < 0)
    {
        printf("open:%s failed!\n",argv[1]);
    }

    BITMAPFILEHEADER bmp_head={0};
    BITMAPINFOHEADER bmp_info_head={0};
    char *buff[14];
    if((read(fd_bmp,(void *)buff,14))<0)
    {
        printf("read data error!\n");
    }

    BYTE *pIndex=(BYTE *)buff;
    bmp_head.bftype=*((unsigned short*)pIndex);
    bmp_head.bfSize=*((unsigned int*)(pIndex+2));
    bmp_head.bfReserved1=*((unsigned short*)(pIndex+6));
    bmp_head.bfReserved2=*((unsigned short*)(pIndex+8));
    bmp_head.bfOffBits=*((unsigned int*)(pIndex+10));

    read(fd_bmp,(void *)&bmp_info_head,sizeof(BITMAPINFOHEADER));

    show_bitmap_head(&bmp_head);
    show_bitmap_headinfo(&bmp_info_head);

    close(fd_bmp);
    return 1;
}


 
