#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/kdev_t.h>
#include<linux/fs.h>
#include <linux/device.h>
#include<linux/cdev.h>
#include<linux/slab.h>                 //kmalloc()
#include<linux/uaccess.h>              //copy_to/from_user() 
#include <linux/ioctl.h>
#include<linux/proc_fs.h>

#define WR_VALUE _IOW('a','a',int32_t*)
#define RD_VALUE _IOR('a','b',int32_t*)

char array[20]="proc_array\n";
static int len = 1; 

int32_t value = 0;
dev_t dev = 0; 
static struct class *dev_class;
static struct cdev simple_cdev;
static struct proc_dir_entry *parent;

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
        return 0;
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
** This function will be called when we write IOCTL on the Device file
*/
static long sim_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
         switch(cmd) {
                case WR_VALUE:
                        if( copy_from_user(&value ,(int32_t*) arg, sizeof(value)) )
                        {
                                pr_err("Data Write : Err!\n");
                        }
                        pr_info("Value = %d\n", value);
                        break;
                case RD_VALUE:
                        if( copy_to_user((int32_t*) arg, &value, sizeof(value)) )
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
 

struct file_operations simple_fops=
{
    .owner      = THIS_MODULE,
    .read       = sim_read,
    .write      = sim_write,
    .open       = sim_open,
    .unlocked_ioctl = sim_ioctl,	
    .release    = sim_release,
};

static int open_proc(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "proc file opend.....\t");
    return 0;
}
static int release_proc(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "proc file released.....\n");
    return 0;
}

/*
** This function will be called when we read the procfs file
*/
static ssize_t read_proc(struct file *filp, char __user *buffer, size_t length,loff_t * offset)
{
    pr_info("proc file reading.....\n");
    if(len)
    {
        len=0;
    }
    else
    {
        len=1;
        return 0;
    }
    
    if( copy_to_user(buffer,array,20) )
    {
        pr_err("Data Send : Err!\n");
    }
 
    return length;;
}
/*
** This function will be called when we write the procfs file
*/
static ssize_t write_proc(struct file *filp, const char *buff, size_t len, loff_t * off)
{
    pr_info("proc file writing.....\n");
    
    if( copy_from_user(array,buff,len) )
    {
        pr_err("Data Write : Err!\n");
    }
    
    return len;
}
static struct proc_ops proc_fops = {
        .proc_open = open_proc,
        .proc_read = read_proc,
        .proc_write = write_proc,
        .proc_release = release_proc
};

/*
** Module Init function
*/

static int __init proc_entry_init(void)
{

	/*Allocating Major number*/

	if((alloc_chrdev_region(&dev, 0, 1, "simple_dev")) <0)
	{
		printk(KERN_INFO "Cannot allocate major number for device 1\n");
        	return -1;
	}

	/*Creating cdev structure*/

	cdev_init(&simple_cdev, &simple_fops);

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

  
        /*Create proc directory. It will create a directory under "/proc" */
        parent = proc_mkdir("new_proc",NULL);
        
        if( parent == NULL )
        {
            pr_info("Error creating proc entry");
            goto r_device;
        }
        
        /*Creating Proc entry under "/proc/new_proc/" */
        proc_create("sj_proc", 0666, parent, &proc_fops);

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

static void __exit proc_entry_exit(void)
{
	proc_remove(parent);
        device_destroy(dev_class,dev);
        class_destroy(dev_class);
	cdev_del(&simple_cdev);
	unregister_chrdev_region(dev, 1);	
	printk(KERN_INFO "Kernel Module Removed Successfully\n");
}
 
module_init(proc_entry_init);
module_exit(proc_entry_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sharon Jerome");
MODULE_DESCRIPTION("A simple proc_entry driver");
MODULE_VERSION("1.0");
