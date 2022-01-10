#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include<linux/slab.h>                 //kmalloc()
#include<linux/uaccess.h>              //copy_to/from_user()
#include <linux/ioctl.h>
#define SIZE 200 
//IOCTL format MACRO 
#define GETSTAT _IOWR('r','a',int32_t*) 

 
int32_t value = 0;
 
dev_t dev = 0;
static struct class *dev_class;
static struct cdev mycdev;
int ret;
struct stats
{
    int size;
    char buff[SIZE];
    int r_w;
};
struct stats *data;

// Function Prototypes
static int      __init mydriver_init(void);
static void     __exit mydriver_exit(void);
static int      my_open(struct inode *inode, struct file *file);
static int      my_release(struct inode *inode, struct file *file);
static ssize_t  my_read(struct file *filp, char __user *Ubuffer, size_t count,loff_t * off);
static ssize_t  my_write(struct file *filp, const char *Ubuffer, size_t count, loff_t * off);
static long     my_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

/*
** File operation sturcture
*/
static struct file_operations fops =
{
        .owner          = THIS_MODULE,
        .read           = my_read,
        .write          = my_write,
        .open           = my_open,
        .unlocked_ioctl = my_ioctl,
        .release        = my_release,
};

/*
** This function will be called when we open the Device file
*/
static int my_open(struct inode *inode, struct file *file)
{
        pr_info("Inside Open...!!!\n");
        return 0;
}

/*
** This function will be called when we close the Device file
*/
static int my_release(struct inode *inode, struct file *file)
{
        pr_info("Device File Closed...!!!\n");
        return 0;
}

/*
** This function will be called when we read the Device file
*/
static ssize_t my_read(struct file *filp, char __user *Ubuffer, size_t count, loff_t *off)
{
        pr_info("Read Function\n");
        return 0;
}

/*
** This function will be called when we write the Device file
*/
static ssize_t my_write(struct file *filp, const char *Ubuffer, size_t count, loff_t *off)
{
        pr_info("Write function\n");
        ret = copy_from_user((char *)data->buff, Ubuffer, count);
        data->size =count;
        data->r_w = 1;

        printk("Kbuffer Data : %s",data->buff);
        printk("Data:%s \n",data->buff);
        return count;
}

/*
** This function will be called when we write IOCTL on the Device file
*/
static long my_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
         switch(cmd) {
                case GETSTAT:
                        if( copy_to_user((struct stats *)arg,data, sizeof(data)) )
                        {
                            pr_err("Data Read : Err!\n");
                        }
                        break;
                default:
                        pr_info("Default\n");
                        break;
        }
        return 0;
}
 
/*
** Module Init function
*/
static int __init mydriver_init(void)
{
        /*Allocating Major number*/
        if((alloc_chrdev_region(&dev, 0, 1, "myexam")) <0){
            pr_err("Cannot allocate major number\n");
            return -1;
        }
        pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
 
        /*Creating cdev structure*/
        cdev_init(&mycdev,&fops);
 
        /*Adding character device to the system*/
        if((cdev_add(&mycdev,dev,1)) < 0){
            pr_err("Cannot add the device to the system\n");
            goto r_class;
        }
 
        /*Creating struct class*/
        if((dev_class = class_create(THIS_MODULE,"exam_class")) == NULL){
            pr_err("Cannot create the struct class\n");
            goto r_class;
        }
 
        /*Creating device*/
        if((device_create(dev_class,NULL,dev,NULL,"myexam")) == NULL){
            pr_err("Cannot create the Device 1\n");
            goto r_device;
        }
        pr_info("Device Driver Insert...Done!!!\n");
        return 0;
 
r_device:
        class_destroy(dev_class);
r_class:
        unregister_chrdev_region(dev,1);
        return -1;
}

/*
** Module exit function
*/
static void __exit mydriver_exit(void)
{
        device_destroy(dev_class,dev);
        class_destroy(dev_class);
        cdev_del(&mycdev);
        unregister_chrdev_region(dev, 1);
        pr_info("Device Driver Remove...Done!!!\n");
}
 
module_init(mydriver_init);
module_exit(mydriver_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rishabh Srivastava");
MODULE_DESCRIPTION("Question1");