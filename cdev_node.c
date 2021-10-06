#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/kdev_t.h>
#include<linux/fs.h>
#include <linux/device.h>
 
dev_t dev = 0; 
static struct class *dev_class;
static struct cdev simple_cdev;

struct file_operations simple_fops=
{
    .owner      = THIS_MODULE,
    .read       = sim_read,
    .write      = sim_write,
    .open       = sim_open,
    .release    = sim_release,
};

/*
** This function will be called when we open the Device file
*/
static int sim_open(struct inode *inode, struct file *file)
{
        pr_info("Driver Open Function Called\n");
        return 0;
}
/*
** This function will be called when we close the Device file
*/
static int sim_release(struct inode *inode, struct file *file)
{
        pr_info("Driver Release Function Called\n");
        return 0;
}
/*
** This function will be called when we read the Device file
*/
static ssize_t sim_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
        pr_info("Driver Read Function Called\n");
        return len;
}
/*
** This function will be called when we write the Device file
*/
static ssize_t sim_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
        pr_info("Driver Write Function Called\n");
        return len;
}

/*
** Module Init function
*/

static int __init cdev_node_init(void)
{

	/*Allocating Major number*/

	if((alloc_chrdev_region(&dev, 0, 1, "simple_dev")) <0)
	{
		printk(KERN_INFO "Cannot allocate major number for device 1\n");
        	return -1;
	}

	/*Creating cdev structure*/

	cdev_init(&simple_cdev, simple_fops);

        /*Adding character device to the system*/

        if((cdev_add(&simple_cdev,dev,1)) < 0){
            pr_err("Cannot add the device to the system\n");
            goto r_class;
        }

        /*Creating struct class*/

        if((dev_class = class_create(THIS_MODULE,"simple_class")) == NULL)
	{
           	 pr_err("Cannot create the struct class for device\n");
           	 goto r_class;
        }
 
        /*Creating device*/

        if((device_create(dev_class,NULL,dev,NULL,"simple_device")) == NULL)
	{
           	 pr_err("Cannot create the Device\n");
           	 goto r_device;
        }

        pr_info("Kernel Module Inserted Successfully...\n");
	printk(KERN_INFO "Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
        return 0;
 
r_device:
        class_destroy(dev_class);
r_class:
        unregister_chrdev_region(dev,1);
        return -1;	
}

/*
** Module Exit function
*/

static void __exit cdev_node_exit(void)
{
        device_destroy(dev_class,dev);
        class_destroy(dev_class);
	cdev_del(&simple_cdev);
	unregister_chrdev_region(dev, 1);	
	printk(KERN_INFO "Kernel Module Removed Successfully\n");
}
 
module_init(cdev_node_init);
module_exit(cdev_node_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sharon Jerome");
MODULE_DESCRIPTION("A simple cdev_node driver");
MODULE_VERSION("1.0");
