#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/err.h>
#include <linux/ioctl.h>

dev_t dev = 0;
static struct class *my_class;
static struct cdev my_cdev;

//Function prototype
static int  __init my_driver_init(void);
static void  __exit my_driver_exit(void);

/******************Driver function**************************/
static int      file_open(struct inode *inode, struct file *file);
static int      file_release(struct inode *inode, struct file *file);
static ssize_t  file_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t  file_write(struct file *filp, const char *buf, size_t len, loff_t * off);
/**************************************************************/

/*File operation structure*/
static struct file_operations fops =
{
        .owner          = THIS_MODULE,
        .read           = file_read,
        .write          = file_write,
        .open           = file_open,
        .release        = file_release,
};

/*This function will be called when we open the Device file*/

static int file_open(struct inode *inode, struct file *file)
{
        pr_info("Device File Opened...!!!\n");
        return 0;
}

/*This function will be called when we close the Device file*/

static int file_release(struct inode *inode, struct file *file)
{
        pr_info("Device File Closed...!!!\n");
        return 0;
}

static ssize_t file_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
        printk("In the Read function....\n");
        return 0;
}

/*This function will be called when we write the Device file*/

static ssize_t file_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
        printk("In the write function....\n");
        return 0;
}

/*Module Init function*/

static int __init my_driver_init(void)
{
        /*Allocating Major number*/
        if((alloc_chrdev_region(&dev, 0, 1, "my_Dev")) <0){
                pr_info("Cannot allocate major number\n");
                return -1;
        }
        pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));

        /*Creating cdev structure*/
        cdev_init(&my_cdev,&fops);

        /*Adding character device to the system*/
        if((cdev_add(&my_cdev,dev,1)) < 0){
            pr_info("Cannot add the device to the system\n");
            goto r_class;
        }

        /*Creating struct class*/
        if(IS_ERR(my_class = class_create(THIS_MODULE,"my_class"))){
            pr_info("Cannot create the struct class\n");
            goto r_class;
        }

        /*Creating device*/
        if(IS_ERR(device_create(my_class,NULL,dev,NULL,"my_device"))){
            pr_info("Cannot create the Device 1\n");
            goto r_device;
        }
r_device:
        class_destroy(my_class);
r_class:
        unregister_chrdev_region(dev,2);
        return -1;
}

/*Module exit function*/
static void __exit my_driver_exit(void)
{
         device_destroy(my_class,dev);
         class_destroy(my_class);
         cdev_del(&my_cdev);
         unregister_chrdev_region(dev, 2);
         pr_info("Device Driver Remove...Done!!!\n");
}

module_init(my_driver_init);
module_exit(my_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tejaswani");

