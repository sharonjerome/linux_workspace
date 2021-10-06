#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/kdev_t.h>
#include<linux/fs.h>
 
dev_t dev = 0; 

/*
** Module Init function
*/

static int __init device_num_init(void)
{
	printk(KERN_INFO "Kernel Module Inserted Successfully\n");

	/*Allocating Major number*/

	if((alloc_chrdev_region(&dev, 0, 1, "simple_dev")) <0)
	{
		printk(KERN_INFO "Cannot allocate major number for device 1\n");
        	return -1;
	}

	printk(KERN_INFO "Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
	return 0;
}

/*
** Module Exit function
*/

static void __exit device_num_exit(void)
{
	printk(KERN_INFO "Kernel Module Removed Successfully\n");
	unregister_chrdev_region(dev, 1);	
}
 
module_init(device_num_init);
module_exit(device_num_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sharon Jerome");
MODULE_DESCRIPTION("A simple device_num driver");
MODULE_VERSION("1.0");
