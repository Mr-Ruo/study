#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>

#define CAMERA_DEV "/dev/video0"
#define ENABLE_MY_LOG (1)
#define BUFFER_COUNT 3

#if (ENABLE_MY_LOG)
#define MY_LOG(fmt,args...) printf("%s:"fmt"\r\n",__FUNCTION__,##args)
#else
#define MY_LOG(fmt,args...) 
#endif

//////////////////////////////////////////////////////////////////
//全局变量声明和定义
//////////////////////////////////////////////////////////////////
struct buffer{
    void* start;
    size_t length;
};

struct buffer* buffers=NULL;
void saveBuffer(struct v4l2_buffer buf);

//////////////////////////////////////////////////////////////////
//方法的定义与实现
//////////////////////////////////////////////////////////////////

/*
 *
 * 获取一帧数据
 * 
 * */
static int read_frame(int fd)
{
    struct v4l2_buffer buf;
    memset(&buf,0,sizeof(buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    if(-1 == ioctl(fd,VIDIOC_DQBUF,&buf))
    {
        MY_LOG("VIDIOC_DQBUF failed!\n");
    }

    saveBuffer(buf);

    if(-1 == ioctl(fd,VIDIOC_QBUF,&buf))
    {
        MY_LOG("VIDIOC_QBUF failed!\n");
    }

    return 0;
}

void saveBuffer(struct v4l2_buffer buf)
{
    int index = buf.index;
    int fd_image=open("1.yuv",O_RDWR | O_CREAT);
    if(write(fd_image,buffers[index].start,buffers[index].length) > 0)
    {
        MY_LOG("saving...\n");
    }

    close(fd_image);
}

int openCamera(){
    
    int fd;
    struct v4l2_capability cap;
    MY_LOG("+");
    if((fd=open(CAMERA_DEV,O_RDWR))>0)
    {
        ioctl(fd,VIDIOC_QUERYCAP,&cap);
        printf("DriverName:%s\nCard Name:%s\nBus info:%s\nDriverVersion:%u.%u.%u\n",
               cap.driver,
               cap.card,
               cap.bus_info,
               (cap.version>>16)&0xff,
               (cap.version>>8)&0xff,
               cap.version&0xff);

        struct v4l2_fmtdesc fmtdesc;
        fmtdesc.index=0;
        fmtdesc.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
        printf("Supportformat:\n");
        while(ioctl(fd,VIDIOC_ENUM_FMT,&fmtdesc)!=-1)
        {
            printf("----%d.%s\n",fmtdesc.index+1,fmtdesc.description);
            fmtdesc.index++;
        }

    }else{
        MY_LOG("openCamera error!\n");
        return -1;
    }
    MY_LOG("-");
    return fd;
}

int initCamera(int fd)
{
    struct v4l2_format fmt;
    struct v4l2_requestbuffers req;

    MY_LOG("+");
    memset(&fmt,0,sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = 640;
    fmt.fmt.pix.height = 480;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
    if(ioctl(fd,VIDIOC_S_FMT,&fmt)==-1){
        MY_LOG("set fmt failed!\n");
    }

    memset(&req,0,sizeof(req));
    req.count = 3;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    ioctl(fd,VIDIOC_REQBUFS,&req);

    int n_buffers=0;
    buffers = calloc(req.count,sizeof(struct buffer));
    for(n_buffers;n_buffers < req.count;++n_buffers){
        struct v4l2_buffer buf;  //驱动中的一帧
        memset(&buf,0,sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = n_buffers;
        //映射到用户空间
        if(-1 == ioctl(fd,VIDIOC_QUERYBUF,&buf)){
            MY_LOG("VIDIOC_QUERYBUF error!\n");
        }

        buffers[n_buffers].length = buf.length;
        buffers[n_buffers].start = mmap(NULL,buf.length,PROT_READ | PROT_WRITE,MAP_SHARED,fd,buf.m.offset);

        if(MAP_FAILED == buffers[n_buffers].start)
        {
            MY_LOG("memory map failed!\n");
        }
        //将申请到的帧缓冲入队列
        if(-1 == ioctl(fd,VIDIOC_QBUF,&buf)){
            MY_LOG("VIDIOC_QBUF failed!\n");
        }
    }

    MY_LOG("-");
    return 0;
}

void start(int fd)
{
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(-1 == ioctl(fd,VIDIOC_STREAMON,&type))
    {
        MY_LOG("start failed!\n");
    }
}

void stop(int fd)
{
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(-1 == ioctl(fd,VIDIOC_STREAMOFF,&type))
    {
        MY_LOG("stop failed!\n");
    }
}

int closeCamera(int fd)
{
    int n_buffers;
    for(n_buffers =0;n_buffers < BUFFER_COUNT;n_buffers++){
        munmap(buffers[n_buffers].start,buffers[n_buffers].length);
    }

    close(fd);
    return 0;
}

/////////////////////////////////////////////////////////////////////
//测试使用
/////////////////////////////////////////////////////////////////////

int main(void)
{
    int fd_cam;
    fd_cam = openCamera();
    initCamera(fd_cam);
    start(fd_cam);
    //sleep(10);
    read_frame(fd_cam);
    stop(fd_cam);
    closeCamera(fd_cam);
    return 0;
}
