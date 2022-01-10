#include<linux/init.h>
#include<linux/module.h>
#include<linux/fcntl.h>
#include<linux/kernel.h>
#include<linux/types.h>
#include<linux/fs.h>
#include<linux/kdev_t.h>
#include<linux/cdev.h>
#include<linux/uaccess.h>
#include<linux/gpio.h>
#include<linux/timer.h>
#include<linux/jiffies.h>

//IOCTL format MACRO 
#define WR_VALUE _IOW('a','a',int32_t*) 
#define RD_VALUE _IOR('a','b',int32_t*)
//Timer Variable*/
//#define TIMEOUT 1000   //milliseconds
unsigned int delay = 3000;
//int32_t value = 0;
int timer_state=0;
int flag=0;
dev_t dev = 0;
int32_t value = 0;
static struct class *dev_class;
static struct cdev etx_cdev;
static unsigned int gpioLED =25;
//Instance of timer 
static struct timer_list etx_timer;
static unsigned int count = 0;
static unsigned int a = 0 ;
//static bool gpipVal = 0 ;
/*
** Function Prototypes
*/
static int      __init etx_driver_init(void);
static void     __exit etx_driver_exit(void);
static long     etx_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
static int      etx_open(struct inode *inode, struct file *file);
static int      etx_release(struct inode *inode, struct file *file);
static ssize_t  etx_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t  etx_write(struct file *filp, const char *buf, size_t len, loff_t * off);
void timer_callback(struct timer_list * data);
static struct file_operations fops =
{
        .owner          = THIS_MODULE,
        .read           = etx_read,
        .write          = etx_write,
        .open           = etx_open,
        .unlocked_ioctl = etx_ioctl,
        .release        = etx_release,
};

static int etx_open(struct inode *inode, struct file *file)
{
        pr_info("Device File Opened...!!!\n");
        return 0;
}

/*
** This function will be called when we close the Device file
*/
static int etx_release(struct inode *inode, struct file *file)
{
        pr_info("Device File Closed...!!!\n");
        return 0;
}

/*
** This function will be called when we read the Device file
*/
static ssize_t etx_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
        pr_info("Read Function\n");
        return 0;
}

/*
** This function will be called when we write the Device file
*/
static ssize_t etx_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
        pr_info("Write function\n");
        return len;
}
/*
** This function will be called when we write IOCTL on the Device file
*/
static int __init etx_driver_init(void)
{
     /*Allocating Major number*/
        if((alloc_chrdev_region(&dev, 0, 1, "rpi_Dev")) <0){
            pr_err("Cannot allocate major number\n");
            return -1;
        }
        pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
 
        /*Creating cdev structure*/
        cdev_init(&etx_cdev,&fops);
 
        /*Adding character device to the system*/
        if((cdev_add(&etx_cdev,dev,1)) < 0){
            pr_err("Cannot add the device to the system\n");
            goto r_class;
        }
 
        /*Creating struct class*/
        if((dev_class = class_create(THIS_MODULE,"rpi_class")) == NULL){
            pr_err("Cannot create the struct class\n");
            goto r_class;
        }
 
        /*Creating device*/
        if((device_create(dev_class,NULL,dev,NULL,"toggle_device")) == NULL){
            pr_err("Cannot create the Device 1\n");
            goto r_device;
        }
        pr_info("Device Driver Insert...Done!!!\n");
        return 0;
 

    if(!gpio_is_valid(gpioLED))
        {
            printk("Invalid GPIO\n");
            return 0;
        } 

    gpio_request(gpioLED,"GPIO_25");
    gpio_direction_output(gpioLED,flag);
    gpio_export(gpioLED,true);

    /* setup your timer to call my_timer_callback */
    timer_setup(&etx_timer, timer_callback, 0);       //If you face some issues and using older kernel version, then you can try setup_timer API(Change Callback function's argument to unsingned long instead of struct timer_list *.
 
    /* setup timer interval to based on TIMEOUT Macro */
    mod_timer(&etx_timer, jiffies + msecs_to_jiffies(delay));

    r_device:
        class_destroy(dev_class);
    r_class:
        unregister_chrdev_region(dev,1);
        return -1;

    return 0;
}

static long etx_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    long result ;
    switch(cmd) {
                case WR_VALUE:
                        result=copy_from_user(&delay ,(int32_t*) arg, sizeof(delay));
                        if(result)
                        {
                            pr_err("Data Write : Err!\n");
                        }
                        pr_info("Value = %d\n", delay);
                        break;
                case RD_VALUE:
                        result=copy_to_user((int32_t*) arg,(int*)&delay, sizeof(delay)); 
                        if(result)
                        {
                            pr_err("Data Read : Err!\n");
                        }
                        break;
                default:
                        pr_info("Default\n");
                        break;
        }
        return result;
}
 
/*
** Module Init function
*/

void timer_callback(struct timer_list * data)
{
    
    
    /* do your timer stuff here */
    int gpioVal= gpio_get_value(gpioLED);
    printk("%d",gpioVal);

    pr_info("Timer Callback function Called [%d]\n",count++);
    printk("Before LOOP flag-->%d\n",flag);
    if(flag==0)
    {
       flag =1;
       gpio_set_value(gpioLED,flag);
       printk("Inside Loop");
       int gpioVal= gpio_get_value(gpioLED);
       printk("inside gpioval :%d\n",gpioVal);   
    }
    else
    {
    flag = 0;
    gpio_set_value(gpioLED,flag);

    }

    mod_timer(&etx_timer, jiffies + msecs_to_jiffies(delay));
    
}



/*
** Module exit function
*/
static void __exit etx_driver_exit(void)
{
        gpio_direction_output(gpioLED,0);
        del_timer(&etx_timer);
        gpio_unexport(gpioLED);
        pr_info("Device Driver Remove...Done!!!\n");
}
 
module_init(etx_driver_init);
module_exit(etx_driver_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("RishabhSrivastava");
MODULE_DESCRIPTION("Led Program with led");
