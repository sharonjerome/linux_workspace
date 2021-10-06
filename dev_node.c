#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/kdev_t.h>
#include<linux/fs.h>
#include <linux/device.h>
 
dev_t dev = 0; 
static struct class *dev_class;

/*
** Module Init function
*/

static int __init dev_node_init(void)
{

	/*Allocating Major number*/

	if((alloc_chrdev_region(&dev, 0, 1, "simple_dev")) <0)
	{
		printk(KERN_INFO "Cannot allocate major number for device 1\n");
        	return -1;
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

static void __exit dev_node_exit(void)
{
        device_destroy(dev_class,dev);
        class_destroy(dev_class);
	unregister_chrdev_region(dev, 1);	
	printk(KERN_INFO "Kernel Module Removed Successfully\n");
}
 
module_init(dev_node_init);
module_exit(dev_node_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sharon Jerome");
MODULE_DESCRIPTION("A simple dev_node driver");
MODULE_VERSION("1.0");
