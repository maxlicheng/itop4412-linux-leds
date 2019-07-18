/**********************************************************************************************************
** 文件名		:leds.c
** 作者			:maxlicheng<licheng.chn@outlook.com>
** 作者github	:https://github.com/maxlicheng
** 作者博客		:https://www.maxlicheng.com/	
** 生成日期		:2019-07-18
** 描述			:iTop4412全能板led驱动程序
************************************************************************************************************/
#include <linux/module.h>			//模块头文件
#include <linux/kernel.h>			//内核头文件
#include <linux/init.h>				//内核初始化

#include <linux/fs.h>				//字符设备函数
#include <linux/cdev.h>				//字符设备描述
#include <linux/kdev_t.h>			//系列设备号处理宏

#include <linux/slab.h>				//内存分配头文件
#include <linux/device.h>			//设备类节点头文件

#include <linux/gpio.h>				//GPIO操作头文件
#include <mach/gpio.h>				//
#include <plat/gpio-cfg.h>			//

#include <linux/timer.h>  			//内核定时器头文件
#include <asm/uaccess.h>

#define DEVICE_NAME 	"leds"		//字符设备名称
#define DEVICE_MINOR_NUM 	2		//字符设备数量

static char *leds_node_name[] = {	//设备节点名称
	"led0",
	"led1",
};

#define DEV_MAJOR	0				//主设备号
#define DEV_MINOR	0				//次设备号，0为自动分配

static int leds_major = DEV_MAJOR;	//主设备号变量
static int leds_minor = DEV_MINOR;	//次设备号变量

static dev_t leds_dev;				//设备号

struct cdev *leds_cdev;				//字符设备结构体变量

static struct class *leds_class;	//类结构体变量

static int leds_gpio[] = {			//GPIO引脚定义
	EXYNOS4_GPC0(2),
	EXYNOS4_GPD0(0),
};

typedef struct {					//GPIO操作相关结构体
    unsigned char status[2];		//状态标志
    unsigned int  freq[2];			//翻转频率
	unsigned char tflag[2];			//定时器标志
	struct timer_list timer[2];		//内核定时器结构体
    int gpio[2];					//GPIO
}_ledx_info;

_ledx_info leds_info;				//结构体变量定义

#define LED_IOC_SET  		_IO ('k', 0)						//IO操作cmd命令，与应用程序对应
#define LED_IOC_GET 		_IO ('k', 1)
#define LED_IOC_SET_BLINK 	_IO ('k', 2)
#define LED_IOC_GET_BLINK	_IO ('k', 3)

static void led0_timer_function(unsigned long data)
{
	leds_info.status[0] = (~(leds_info.status[0]))&0x01;
	gpio_set_value(leds_info.gpio[0], leds_info.status[0]);
	leds_info.timer[0].expires = jiffies+HZ/leds_info.freq[0];
	add_timer(&leds_info.timer[0]);
}

static void led1_timer_function(unsigned long data)
{
	leds_info.status[1] = (~(leds_info.status[1]))&0x01;
	gpio_set_value(leds_info.gpio[1], leds_info.status[1]);
	leds_info.timer[1].expires = jiffies+HZ/leds_info.freq[1];
	add_timer(&leds_info.timer[1]);
}

static int leds_open(struct inode *inode, struct file *file)
{
	int ret = 0;
	printk(KERN_EMERG "leds ops open is ok\n");
	return ret;
}

static int leds_release(struct inode *inode, struct file *file)
{
	int ret = 0;
	int minor = MINOR(file->f_dentry->d_inode->i_rdev);
	printk(KERN_EMERG "leds ops release is ok\n");
	gpio_free(leds_info.gpio[minor]);
	return ret;
}

static ssize_t leds_read(struct file *file, char __user *buff, size_t count, loff_t *offp)
{
	int ret = 0;
	int minor = MINOR(file->f_dentry->d_inode->i_rdev);
	copy_to_user(buff, (const void *)&leds_info.status[minor], 1); 
	printk("leds read is ok, led_status[%d] is %d\n", minor, leds_info.status[minor]);
	return ret;
}

static ssize_t leds_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	int ret = 0;
	char val = 0;
	int minor = MINOR(file->f_dentry->d_inode->i_rdev);
	if(leds_info.tflag[minor] == 1)
	{
		leds_info.freq[minor] = 0;
		leds_info.tflag[minor] = 0;
		del_timer(&leds_info.timer[minor]);
	}
	copy_from_user(&val, buf, 1);
	leds_info.status[minor] = val;
	gpio_set_value(leds_info.gpio[minor], leds_info.status[minor]);
	printk("leds write is ok, led_status[%d] is %d\n", minor, leds_info.status[minor]);
	return ret;
}

static long leds_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	long ret = 0;
	int *parg = NULL;
	
	int minor = MINOR(file->f_dentry->d_inode->i_rdev);
	
	printk(KERN_EMERG "%s: minor = %d, cmd = %d, arg = %d\n", __FUNCTION__, minor, cmd, (int)arg);
	
	switch(cmd) {
		case LED_IOC_SET:
			if(arg < 2)
			{
				if(leds_info.tflag[minor] == 1)
				{
					leds_info.freq[minor] = 0;
					leds_info.tflag[minor] = 0;
					del_timer(&leds_info.timer[minor]);
				}
				leds_info.status[minor] = arg;
				gpio_set_value(leds_info.gpio[minor], leds_info.status[minor]);
				printk("LED_IOC_SET leds_info.status[%d] is %d\n", minor, leds_info.status[minor]);
			}
			break;
		case LED_IOC_GET:
			parg = arg;
			*parg = leds_info.status[minor];
			printk("LED_IOC_GET leds_info.status[%d] is %d\n", minor, parg[0]);
			break;
		case LED_IOC_SET_BLINK:
			leds_info.freq[minor] = arg;
			if(arg == 0)
			{
				leds_info.freq[minor] = 0;
				leds_info.tflag[minor] = 0;
				del_timer(&leds_info.timer[minor]);
				break;
			}
			if(leds_info.tflag[minor] == 0)
			{
				leds_info.tflag[minor] = 1;
				leds_info.timer[minor].expires = jiffies+HZ/leds_info.freq[minor];
				add_timer(&leds_info.timer[minor]);
			}
			printk("LED_IOC_SET_BLINK leds_info.freq_val[%d] is %d\n", minor, leds_info.freq[minor]);
			break;
		case LED_IOC_GET_BLINK:
			parg = arg;
			*parg = leds_info.freq[minor];
			printk("LED_IOC_GET_BLINK leds_info.freq_val[%d] is %d\n", minor, parg[0]);
			break;
		default:
			return -EINVAL;
	}
	
	return ret;
}	

struct file_operations leds_fops = {
	.owner = THIS_MODULE,
	.open  = leds_open,
	.release = leds_release,
	.read  = leds_read,
	.write = leds_write,
	.unlocked_ioctl = leds_ioctl,
};

static __init int leds_init(void)
{
	int ret = 0;
	int i;
	
	ret = alloc_chrdev_region(&leds_dev, leds_minor, DEVICE_MINOR_NUM, DEVICE_NAME);	//动态分配
	if(ret < 0){
		printk(KERN_EMERG "register_chrdev_region req %d is failed!\n", DEV_MAJOR);
		return ret;
	}
	
	leds_major = MAJOR(leds_dev);	//主设备号
	leds_minor = MINOR(leds_dev);	//次设备号
	
	printk(KERN_EMERG "leds chrdev major=%d, minor=%d\n", leds_major, leds_minor);
	
	leds_class = class_create(THIS_MODULE,DEVICE_NAME);		//类节点创建
	
	leds_cdev = kmalloc(DEVICE_MINOR_NUM * sizeof(struct cdev), GFP_KERNEL);	//申请内存
	if(leds_cdev == NULL)
	{
		printk(KERN_EMERG "kmalloc failed");
		unregister_chrdev_region(leds_dev, DEVICE_MINOR_NUM);
		return -ENOMEM;
	}
	
	//memset(leds_cdev, 0, DEVICE_MINOR_NUM * sizeof(struct dev_cdev));
	
	for(i=0; i<DEVICE_MINOR_NUM; i++){
		leds_info.gpio[i] = leds_gpio[i];
		ret = gpio_request(leds_info.gpio[i], leds_node_name[i]);

		s3c_gpio_cfgpin(leds_info.gpio[i], S3C_GPIO_OUTPUT);	//配置为输出
		gpio_set_value(leds_info.gpio[i], 0);					//设置初始值为0

		leds_info.tflag[i] = 0;
		init_timer(&leds_info.timer[i]);
		leds_info.timer[i].data = 0;
		switch(i){
			case 0:
				leds_info.timer[0].function = led0_timer_function;
				break;
			case 1:
				leds_info.timer[1].function = led1_timer_function;
				break;
			default:
				break;
		}
		
		cdev_init(&leds_cdev[i], &leds_fops);				//字符类设备初始化
		
		leds_cdev[i].owner = THIS_MODULE;
		leds_cdev[i].ops = &leds_fops;
		
		ret = cdev_add(&leds_cdev[i], MKDEV(leds_major, leds_minor+i), 1);	//注册到设备
		
		device_create(leds_class, NULL, MKDEV(leds_major, leds_minor+i), NULL, leds_node_name[i]);	//设备节点创建
		
		if(ret < 0){
			printk(KERN_EMERG "cdev_add %d failed!\n", i);
		}
		else{
			printk(KERN_EMERG "cdev_add %d success!\n", i);
		}
	}
	
	return ret;
}

static __exit void leds_exit(void)
{
	int i;
	for(i=0; i<DEVICE_MINOR_NUM; i++){
		gpio_set_value(leds_info.gpio[i], 0);
		gpio_free(leds_info.gpio[i]);
		del_timer(&leds_info.timer[i]);
				
		cdev_del(&leds_cdev[i]);										//注销设备
		device_destroy(leds_class, MKDEV(leds_major, leds_minor+i));	//注销设备节点
	}
	class_destroy(leds_class);											//类节点注销
	kfree(leds_cdev);													//释放内存
	unregister_chrdev_region(leds_dev, DEVICE_MINOR_NUM);				//注销设备号
	printk(KERN_EMERG "leds chrdev exit \n");
}

module_init(leds_init);
module_exit(leds_exit);

MODULE_LICENSE("GPL");
