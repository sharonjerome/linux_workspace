obj-m := hello_world_driver.o

obj-m := device_num.o

obj-m := dev_node.o

obj-m := cdev_node.o

obj-m := driver_node.o

obj-m := ioctl_driver.o

obj-m := proc_entry.o

 
KDIR = /lib/modules/$(shell uname -r)/build
 
all:
	make -C $(KDIR)  M=$(shell pwd) modules
 
clean:
	make -C $(KDIR)  M=$(shell pwd) clean
