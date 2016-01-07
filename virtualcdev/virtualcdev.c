#include<linux/module.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/types.h>
#include<linux/kernel.h>
#include<linux/fs.h>
#include<linux/init.h>
#include<linux/delay.h>
#include<asm/uaccess.h>
#include<asm/io.h>
#include<asm/system.h>
#include<linux/mm.h>
#include<linux/slab.h>

#define GLOBALMEM_SIZE 0x1000
#define DEVICE_NAME "virtualcdev"  /* 加载模块后，执行”cat /proc/devices”命令看到的设备名称 */
#define VIRTUALCDEV_MAJOR 0     /* 主设备号 */
#define MEM_CLEAR 0x1 /* 清零全局内存 */

static int major = VIRTUALCDEV_MAJOR;
static struct class *virtualcdev_class;
struct device *virtualcdev_device;

/* globalmem设备结构体 */
struct virtualcdev_dev
{
	struct cdev cdev; /* cdev结构体 */
	unsigned char mem[GLOBALMEM_SIZE]; /* 全局内存 */
};

struct virtualcdev_dev *virtualcdev_devp; /* 设备结构体指针 */

/* 应用程序对设备文件/dev/virtualcdev执行open(...)时，
 * 就会调用virtualcdev_open函数
 */
static int virtualcdev_open(struct inode *inode, struct file *filp)
{
	filp->private_data = virtualcdev_devp;
	return 0;
}

static int virtualcdev_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static long virtualcdev_ioctl(struct inode *inodep, struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct virtualcdev_dev *devp = filp->private_data; /* 获得设备结构体指针 */
	switch(cmd)
	{
	case MEM_CLEAR:
			memset(devp->mem, 0, GLOBALMEM_SIZE);
			printk(KERN_INFO "globalmem is set to zero\n");
			break;
	default:
			return -EINVAL;
	}
	return 0;
}

static ssize_t virtualcdev_read(struct file *filp, char __user *buf, size_t size, loff_t *ppos)
{
	unsigned long p = *ppos;
	unsigned int count = size;
	int ret = 0;
	struct virtualcdev_dev *devp = filp->private_data; /* 获得设备结构体指针 */

	if(p >= GLOBALMEM_SIZE)
		return 0;

	if(count > GLOBALMEM_SIZE - p)
		count = GLOBALMEM_SIZE - p;

	if(copy_to_user(buf, (void *)(devp->mem + p), count))
	{
		ret = -EFAULT;
	}
	else
	{
		*ppos += count;
		ret = count;
	}
	return ret;
}

static ssize_t virtualcdev_write(struct file *filp, const char __user *buf, size_t size ,loff_t * ppos)
{
	unsigned long p = *ppos;
	unsigned int count = size;
	int ret = 0;
	struct virtualcdev_dev *devp = filp->private_data;

	if(p >= GLOBALMEM_SIZE)
		return 0;
	if(count > GLOBALMEM_SIZE - p)
		count = GLOBALMEM_SIZE - p;
	
	if(copy_from_user(devp->mem + p, buf, count))
		ret = -EFAULT;
	else
	{
		*ppos += count;
		ret = count;
		printk(KERN_INFO "written %u bytes(s) from %lu\n", count, p);
	}

	return ret;
}

/* seek文件定位函数 */
static loff_t virtualcdev_llseek(struct file *filp, loff_t offset, int orig)
{
	loff_t ret = 0;
	switch(orig)
	{
	case 0: /* 相对文件开始位置位移 */
		if(offset < 0)
		{
			ret = -EINVAL;
			break;
		}
		if((unsigned int)offset > GLOBALMEM_SIZE)
		{
			ret = -EINVAL;
			break;
		}
		filp->f_pos = (unsigned int) offset;
		ret = filp->f_pos;
		break;
	case 1: /* 相对文件当前位置偏移 */
		if((filp->f_pos + offset) < 0)
		{
			ret = -EINVAL;
			break;
		}
		if((filp->f_pos + offset) > GLOBALMEM_SIZE)
		{
			ret = -EINVAL;
			break;
		}
		filp->f_pos += offset;
		ret = filp->f_pos; 
		break;
	default:
		ret = -EINVAL;
		break;
	}
	return ret;
}

/* 这个结构是字符设备驱动程序的核心 
 * 当应用程序操作设备文件时所调用的open、read、write等函数，
 * 最终会调用这个结构中指定的对应函数
 */
static struct file_operations virtualcdev_fops = {
	.owner = THIS_MODULE,    /* 这是一个宏，推向编译模块时自动创建的__this_module变量 */
	.open = virtualcdev_open,     
	.read = virtualcdev_read,
	.write	= virtualcdev_write,
	.llseek = virtualcdev_llseek,
	.release = virtualcdev_release,
	.unlocked_ioctl = virtualcdev_ioctl
};

/*
 * 执行insmod命令时就会调用这个函数 
 */
static int __init virtualcdev_init(void)
{
	dev_t devno = MKDEV(major, 0);

	if(major)
		major = register_chrdev_region(devno, 1, "virtualcdev");
	else
	{
		major = alloc_chrdev_region(&devno, 0, 1, "virtualcdev");
		major = MAJOR(devno);
	}

	virtualcdev_class = class_create(THIS_MODULE, "virtualcdev");
	virtualcdev_device = device_create(virtualcdev_class, 0, MKDEV(major, 0),
					  0, "%s", DEVICE_NAME);

	if (IS_ERR(virtualcdev_device)) {
		printk("device create err\n");
	}
	if(major < 0)
		return major;

	virtualcdev_devp = kmalloc(sizeof(struct virtualcdev_dev), GFP_KERNEL);
	if(!virtualcdev_devp)
	{
		major = -ENOMEM;
		goto fail_malloc;
	}
	
	memset(virtualcdev_devp, 0, sizeof(struct virtualcdev_dev));
	cdev_init(&(virtualcdev_devp->cdev), &virtualcdev_fops);
	virtualcdev_devp->cdev.owner = THIS_MODULE;
	cdev_add(&(virtualcdev_devp->cdev), devno, 1);

	return 0;

fail_malloc: unregister_chrdev_region(devno, 1);
	return major;
}

/*
 * 执行rmmod命令时就会调用这个函数 
 */
static void __exit virtualcdev_exit(void)
{
	device_destroy(virtualcdev_class, MKDEV(major, 0));
	class_destroy(virtualcdev_class);
	cdev_del(&virtualcdev_devp->cdev);
	kfree(virtualcdev_devp);
	unregister_chrdev_region(MKDEV(major, 0), 1);
}

/* 这两行指定驱动程序的初始化函数和卸载函数 */
module_init(virtualcdev_init);
module_exit(virtualcdev_exit);

/* 描述驱动程序的一些信息 */
MODULE_AUTHOR("author");
MODULE_VERSION("0.1.0");
MODULE_DESCRIPTION("virtualcdev Driver");
MODULE_LICENSE("GPL");
