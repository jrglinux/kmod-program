#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include<linux/slab.h>                 //kmalloc()
#include<linux/uaccess.h>              //copy_to/from_user()
#include<linux/sysfs.h> 
#include<linux/kobject.h> 
#include <linux/interrupt.h>
#include <asm/io.h>
#include <asm/hw_irq.h>
#include <linux/workqueue.h>            // Required for workqueues
 
 
#define IRQ_NO 11
 
 
void workqueue_fn(struct work_struct *work); 
 
/*Creating work by Static Method */
DECLARE_WORK(workqueue,workqueue_fn);
 
/*Workqueue Function*/
void workqueue_fn(struct work_struct *work)
{
        printk(KERN_INFO "Executing Workqueue Function\n");
}
 
 
//Interrupt handler for IRQ 11. 
static irqreturn_t irq_handler(int irq,void *dev_id) {
	if(0 == strcmp(dev_id, "workq_device")){
        	printk(KERN_INFO "Shared IRQ: Interrupt Occurred");
        	schedule_work(&workqueue);
	}
        
        return IRQ_HANDLED;
}
 
 
volatile int workq_value = 0;
 
 
dev_t dev = 0;
static struct class *dev_class;
static struct cdev workq_cdev;
struct kobject *kobj_ref;
/*
** Function Prototypes
*/
static int __init workq_driver_init(void);
static void __exit workq_driver_exit(void);
 
/*************** Driver Fuctions **********************/
static int workq_open(struct inode *inode, struct file *file);
static int workq_release(struct inode *inode, struct file *file);
static ssize_t workq_read(struct file *filp, 
                char __user *buf, size_t len,loff_t * off);
static ssize_t workq_write(struct file *filp, 
                const char *buf, size_t len, loff_t * off);
 
/*************** Sysfs Fuctions **********************/
static ssize_t sysfs_show(struct kobject *kobj, 
                struct kobj_attribute *attr, char *buf);
static ssize_t sysfs_store(struct kobject *kobj, 
                struct kobj_attribute *attr,const char *buf, size_t count);
 
struct kobj_attribute workq_attr = __ATTR(workq_value, 0660, sysfs_show, sysfs_store);
/*
** File operation sturcture
*/
static struct file_operations fops =
{
        .owner          = THIS_MODULE,
        .read           = workq_read,
        .write          = workq_write,
        .open           = workq_open,
        .release        = workq_release,
};
/*
** This function will be called when we read the sysfs file
*/ 
static ssize_t sysfs_show(struct kobject *kobj, 
                struct kobj_attribute *attr, char *buf)
{
        printk(KERN_INFO "Sysfs - Read!!!\n");
        return sprintf(buf, "%d", workq_value);
}
/*
** This function will be called when we write the sysfsfs file
*/
static ssize_t sysfs_store(struct kobject *kobj, 
                struct kobj_attribute *attr,const char *buf, size_t count)
{
        printk(KERN_INFO "Sysfs - Write!!!\n");
        sscanf(buf,"%d",&workq_value);
        return count;
}
/*
** This function will be called when we open the Device file
*/  
static int workq_open(struct inode *inode, struct file *file)
{
        printk(KERN_INFO "Device File Opened...!!!\n");
        return 0;
}
/*
** This function will be called when we close the Device file
*/  
static int workq_release(struct inode *inode, struct file *file)
{
        printk(KERN_INFO "Device File Closed...!!!\n");
        return 0;
}
/*
** This function will be called when we read the Device file
*/
static ssize_t workq_read(struct file *filp, 
                char __user *buf, size_t len, loff_t *off)
{
        struct irq_desc *desc;
        printk(KERN_INFO "Read function\n");
        desc = irq_to_desc(11);
        if (!desc)
        {
            return -EINVAL;
        }
        __this_cpu_write(vector_irq[59], desc);
        asm("int $0x3B");  // Corresponding to irq 11
        return 0;
}
/*
** This function will be called when we write the Device file
*/
static ssize_t workq_write(struct file *filp, 
                const char __user *buf, size_t len, loff_t *off)
{
        printk(KERN_INFO "Write Function\n");
        return len;
}
 
/*
** Module Init function
*/
static int __init workq_driver_init(void)
{
        /*Allocating Major number*/
        if((alloc_chrdev_region(&dev, 0, 1, "workq_Dev")) <0){
                printk(KERN_INFO "Cannot allocate major number\n");
                return -1;
        }
        printk(KERN_INFO "Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
 
        /*Creating cdev structure*/
        cdev_init(&workq_cdev,&fops);
 
        /*Adding character device to the system*/
        if((cdev_add(&workq_cdev,dev,1)) < 0){
            printk(KERN_INFO "Cannot add the device to the system\n");
            goto r_class;
        }
 
        /*Creating struct class*/
        if((dev_class = class_create(THIS_MODULE,"workq_class")) == NULL){
            printk(KERN_INFO "Cannot create the struct class\n");
            goto r_class;
        }
 
        /*Creating device*/
        if((device_create(dev_class,NULL,dev,NULL,"workq_device")) == NULL){
            printk(KERN_INFO "Cannot create the Device 1\n");
            goto r_device;
        }
 
        /*Creating a directory in /sys/kernel/ */
        kobj_ref = kobject_create_and_add("workq_sysfs",kernel_kobj);
 
        /*Creating sysfs file for workq_value*/
        if(sysfs_create_file(kobj_ref,&workq_attr.attr)){
                printk(KERN_INFO"Cannot create sysfs file......\n");
                goto r_sysfs;
        }
        if (request_irq(IRQ_NO, irq_handler, IRQF_SHARED, "workq_device", "workq_device")) {
            printk(KERN_INFO "my_device: cannot register IRQ ");
                    goto irq;
        }
        printk(KERN_INFO "Device Driver Insert...Done!!!\n");
        return 0;
 
irq:
        free_irq(IRQ_NO,(void *)(irq_handler));
 
r_sysfs:
        kobject_put(kobj_ref); 
        sysfs_remove_file(kernel_kobj, &workq_attr.attr);
 
r_device:
        class_destroy(dev_class);
r_class:
        unregister_chrdev_region(dev,1);
        cdev_del(&workq_cdev);
        return -1;
}
/*
** Module exit function
*/ 
static void __exit workq_driver_exit(void)
{
        free_irq(IRQ_NO,(void *)(irq_handler));
        kobject_put(kobj_ref); 
        sysfs_remove_file(kernel_kobj, &workq_attr.attr);
        device_destroy(dev_class,dev);
        class_destroy(dev_class);
        cdev_del(&workq_cdev);
        unregister_chrdev_region(dev, 1);
        printk(KERN_INFO "Device Driver Remove...Done!!!\n");
}
 
module_init(workq_driver_init);
module_exit(workq_driver_exit);
 
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Linux device driver (Global Workqueue - Static method)");
MODULE_VERSION("1.10");
