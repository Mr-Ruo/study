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
#define DEVICE_NAME "virtualcdev"  /* ����ģ���ִ�С�cat /proc/devices����������豸���� */
#define VIRTUALCDEV_MAJOR 0     /* ���豸�� */
#define MEM_CLEAR 0x1 /* ����ȫ���ڴ� */

static int major = VIRTUALCDEV_MAJOR;
static struct class *virtualcdev_class;
struct device *virtualcdev_device;

/* globalmem�豸�ṹ�� */
struct virtualcdev_dev
{
	struct cdev cdev; /* cdev�ṹ�� */
	unsigned char mem[GLOBALMEM_SIZE]; /* ȫ���ڴ� */
};

struct virtualcdev_dev *virtualcdev_devp; /* �豸�ṹ��ָ�� */

/* Ӧ�ó�����豸�ļ�/dev/virtualcdevִ��open(...)ʱ��
 * �ͻ����virtualcdev_open����
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
	struct virtualcdev_dev *devp = filp->private_data; /* ����豸�ṹ��ָ�� */
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
	struct virtualcdev_dev *devp = filp->private_data; /* ����豸�ṹ��ָ�� */

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

/* seek�ļ���λ���� */
static loff_t virtualcdev_llseek(struct file *filp, loff_t offset, int orig)
{
	loff_t ret = 0;
	switch(orig)
	{
	case 0: /* ����ļ���ʼλ��λ�� */
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
	case 1: /* ����ļ���ǰλ��ƫ�� */
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

/* ����ṹ���ַ��豸��������ĺ��� 
 * ��Ӧ�ó�������豸�ļ�ʱ�����õ�open��read��write�Ⱥ�����
 * ���ջ��������ṹ��ָ���Ķ�Ӧ����
 */
static struct file_operations virtualcdev_fops = {
	.owner = THIS_MODULE,    /* ����һ���꣬�������ģ��ʱ�Զ�������__this_module���� */
	.open = virtualcdev_open,     
	.read = virtualcdev_read,
	.write	= virtualcdev_write,
	.llseek = virtualcdev_llseek,
	.release = virtualcdev_release,
	.unlocked_ioctl = virtualcdev_ioctl
};

/*
 * ִ��insmod����ʱ�ͻ����������� 
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
 * ִ��rmmod����ʱ�ͻ����������� 
 */
static void __exit virtualcdev_exit(void)
{
	device_destroy(virtualcdev_class, MKDEV(major, 0));
	class_destroy(virtualcdev_class);
	cdev_del(&virtualcdev_devp->cdev);
	kfree(virtualcdev_devp);
	unregister_chrdev_region(MKDEV(major, 0), 1);
}

/* ������ָ����������ĳ�ʼ��������ж�غ��� */
module_init(virtualcdev_init);
module_exit(virtualcdev_exit);

/* �������������һЩ��Ϣ */
MODULE_AUTHOR("author");
MODULE_VERSION("0.1.0");
MODULE_DESCRIPTION("virtualcdev Driver");
MODULE_LICENSE("GPL");
