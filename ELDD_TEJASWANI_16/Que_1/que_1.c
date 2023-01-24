
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/semaphore.h>
#include <linux/wait.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/err.h>
#include <linux/kthread.h>

#define mem_size        50	//kernel buffer size

uint32_t read_count = 0;
dev_t dev = 0;
static struct class *my_class;
static struct cdev my_cdev;
static struct task_struct *wait_thread;
uint8_t *kernel_buffer;
int wait_queue_flag = 0;

DECLARE_WAIT_QUEUE_HEAD(wait_queue_hcwq);

//Function prototype
static int  __init my_driver_init(void);
static void  __exit my_driver_exit(void);

/******************Driver function**************************/
static int      file_open(struct inode *inode, struct file *file);
static int      file_release(struct inode *inode, struct file *file);
static ssize_t  file_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t  file_write(struct file *filp, const char *buf, size_t len, loff_t * off);
/**************************************************************/

/*File Operations structure*/

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
        printk("Device File Opened...!!!\n");
        return 0;
}

/*This function will be called when we close the Device file*/

static int file_release(struct inode *inode, struct file *file)
{
        printk("Device File Closed...!!!\n");
        return 0;
}

/*This function will be called when we read the Device file*/

static ssize_t file_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
	printk("In the Read function....\n");
        //Copy the data from the kernel space to the user-space
        if( copy_to_user(buf, kernel_buffer, mem_size) )
        {
                pr_err("Data Read : Err!\n");
        }
        printk("Data Read : Done!\n");
	 wait_queue_flag = 1;
         wake_up_interruptible(&wait_queue_hcwq);

        return mem_size;
}

/*This function will be called when we write the Device file*/

static ssize_t file_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
	printk("In the write function....\n");
        //Copy the data to kernel space from the user-space
        if( copy_from_user(kernel_buffer, buf, len) )
        {
                pr_err("Data Write : Err!\n");
        }
        printk("Data Write : Done!\n");
        return len;
}

/*Thread function*/
static int wait_function(void *unused)
{
    while(1)
     {
        printk("Waiting for event.....\n");
        wait_event_interruptible(wait_queue_hcwq, wait_queue_flag != 0);
        if(wait_queue_flag == 2)
         {
            printk("Event came from Exit function\n");
            return 0;
         }
        printk("Event came from Read Function: %d",++read_count);
        wait_queue_flag = 0;
     }
    do_exit(0);
    return 0;
}


/*Module Init function*/

static int __init my_driver_init(void)
{
        /*Allocating Major number*/
        if((alloc_chrdev_region(&dev, 0, 1, "my_Dev")) <0){
                printk("Cannot allocate major number\n");
                return -1;
        }
        pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));

        /*Creating cdev structure*/
        cdev_init(&my_cdev,&fops);

        /*Adding character device to the system*/
        if((cdev_add(&my_cdev,dev,1)) < 0){
            printk("Cannot add the device to the system\n");
            goto r_class;
        }

        /*Creating struct class*/
        if(IS_ERR(my_class = class_create(THIS_MODULE,"my_class"))){
            printk("Cannot create the struct class\n");
            goto r_class;
        }

        /*Creating device*/
        if(IS_ERR(device_create(my_class,NULL,dev,NULL,"my_device"))){
            printk("Cannot create the Device 1\n");
            goto r_device;
        }

	  //Create the kernel thread with name
	 wait_thread = kthread_create(wait_function, NULL, "WaitThread");
   	 if(wait_thread)
     	{
        	printk("thread created Successfully!!!..\n");
        	wake_up_process(wait_thread);
     	}
    	else
     	{
        	printk("Thread Creation Failed..\n");
     	}

        /*Creating Physical memory*/
        if((kernel_buffer = kmalloc(mem_size , GFP_KERNEL)) == 0){
            printk("Cannot allocate memory in kernel\n");
            goto r_device;
        }
        strcpy(kernel_buffer, "Hello_World");

        printk("Device Driver Insert...Done!!!\n");
        return 0;

r_device:
        class_destroy(my_class);
r_class:
        unregister_chrdev_region(dev,2);
        return -1;
}

/*Module exit function*/
static void __exit my_driver_exit(void)
{
	 wait_queue_flag = 2;
	 wake_up_interruptible(&wait_queue_hcwq);
         kfree(kernel_buffer);
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
