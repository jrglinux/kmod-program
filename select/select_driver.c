#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>                 //kmalloc()
#include <linux/uaccess.h>              //copy_to/from_user()
#include <linux/kthread.h>
#include <linux/wait.h>                 //Required for the wait queues
#include <linux/poll.h>
#include <linux/sysfs.h> 
#include <linux/kobject.h>

//Waitqueue
DECLARE_WAIT_QUEUE_HEAD(wait_queue_select_test_data);

dev_t dev = 0;
static struct class *dev_class;
static struct cdev select_test_cdev;
struct kobject *kobj_ref;
static bool can_write = false;
static bool can_read  = false;
static char select_test_value[20];
/*
** Function Prototypes
*/
static int      __init select_test_driver_init(void);
static void     __exit select_test_driver_exit(void);
 
/*************** Driver Fuctions **********************/
static int      select_test_open(struct inode *inode, struct file *file);
static int      select_test_release(struct inode *inode, struct file *file);
static ssize_t  select_test_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t  select_test_write(struct file *filp, const char *buf, size_t len, loff_t * off);
static unsigned int select_test_poll(struct file *filp, struct poll_table_struct *wait);
/*************** Sysfs Fuctions **********************/
static ssize_t  sysfs_show(struct kobject *kobj, 
                        struct kobj_attribute *attr, char *buf);
static ssize_t  sysfs_store(struct kobject *kobj, 
                        struct kobj_attribute *attr,const char *buf, size_t count);
struct kobj_attribute select_test_attr = __ATTR(select_test_value, 0660, sysfs_show, sysfs_store);
/*
** File operation sturcture
*/
static struct file_operations fops =
{
  .owner          = THIS_MODULE,
  .read           = select_test_read,
  .write          = select_test_write,
  .open           = select_test_open,
  .release        = select_test_release,
  .poll           = select_test_poll
};
/*
** This function will be called when we read the sysfs file
*/
static ssize_t sysfs_show(struct kobject *kobj, 
                          struct kobj_attribute *attr, 
                          char *buf)
{
  pr_info("Sysfs Show - Write Permission Granted!!!\n");
  
  can_write = true;
  
  //wake up the waitqueue
  wake_up(&wait_queue_select_test_data);
  
  return sprintf(buf, "%s", "Success\n");
}
/*
** This function will be called when we write the sysfsfs file
*/
static ssize_t sysfs_store(struct kobject *kobj, 
                           struct kobj_attribute *attr,
                           const char *buf, 
                           size_t count)
{
  pr_info("Sysfs Store - Read Permission Granted!!!\n");
  
  strcpy(select_test_value, buf);
  
  can_read = true;
  //wake up the waitqueue
  wake_up(&wait_queue_select_test_data);
  
  return count;
}
/*
** This function will be called when we open the Device file
*/
static int select_test_open(struct inode *inode, struct file *file)
{
  pr_info("Device File Opened...!!!\n");
  return 0;
}
/*
** This function will be called when we close the Device file
*/
static int select_test_release(struct inode *inode, struct file *file)
{
  pr_info("Device File Closed...!!!\n");
  return 0;
}
/*
** This function will be called when we read the Device file
*/
static ssize_t select_test_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
  pr_info("Read Function : select_test_value = %s\n",select_test_value);   
  
  len = strlen(select_test_value);
  
  strcpy(buf, select_test_value);
  
#if 0  
  if( copy_to_user(buf, select_test_value, len) > 0)
  {
    pr_err("ERROR: Not all the bytes have been copied to user\n");
  }
#endif
  
  return 0;
}
/*
** This function will be called when we write the Device file
*/
static ssize_t select_test_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
  strcpy(select_test_value, buf);
  
  pr_info("Write function : select_test_value = %s\n", select_test_value);
  return len;
}
/*
** This function will be called when app calls the poll function
*/
static unsigned int select_test_poll(struct file *filp, struct poll_table_struct *wait)
{
  __poll_t mask = 0;
  
  poll_wait(filp, &wait_queue_select_test_data, wait);
  pr_info("Poll function\n");
  
  if( can_read )
  {
    can_read = false;
    mask |= ( POLLIN | POLLRDNORM );
  }
  
  if( can_write )
  {
    can_write = false;
    mask |= ( POLLOUT | POLLWRNORM );
  }
    
  return mask;
}
 
/*
** Module Init function
*/
static int __init select_test_driver_init(void)
{
  /*Allocating Major number*/
  if((alloc_chrdev_region(&dev, 0, 1, "select_test_Dev")) <0)
  {
    pr_err("Cannot allocate major number\n");
    return -1;
  }
  pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
  
  /*Creating cdev structure*/
  cdev_init(&select_test_cdev,&fops);
  select_test_cdev.owner = THIS_MODULE;
  select_test_cdev.ops = &fops;
  
  /*Adding character device to the system*/
  if((cdev_add(&select_test_cdev,dev,1)) < 0)
  {
    pr_err("Cannot add the device to the system\n");
    goto r_class;
  }
 
  /*Creating struct class*/
  if((dev_class = class_create(THIS_MODULE,"select_test_class")) == NULL)
  {
    pr_err("Cannot create the struct class\n");
    goto r_class;
  }
        
  /*Creating device*/
  if((device_create(dev_class,NULL,dev,NULL,"select_test_device")) == NULL)
  {
    pr_err("Cannot create the Device 1\n");
    goto r_device;
  }
  
  /*Creating a directory in /sys/kernel/ */
  kobj_ref = kobject_create_and_add("select_test_sysfs",kernel_kobj);
  /*Creating sysfs file for select_test_value*/
  if(sysfs_create_file(kobj_ref,&select_test_attr.attr))
  {
    printk(KERN_INFO"Cannot create sysfs file......\n");
    goto r_sysfs;
  }
  
  //Initialize wait queue
  //init_waitqueue_head(&wait_queue_select_test_data);
  
  pr_info("Select Driver Insert...Done!!!\n");
  return 0;
r_sysfs:
  kobject_put(kobj_ref); 
  sysfs_remove_file(kernel_kobj, &select_test_attr.attr);
r_device:
  class_destroy(dev_class);
r_class:
  unregister_chrdev_region(dev,1);
  return -1;
}
/*
** Module exit function
*/ 
static void __exit select_test_driver_exit(void)
{
  kobject_put(kobj_ref); 
  sysfs_remove_file(kernel_kobj, &select_test_attr.attr);
  device_destroy(dev_class,dev);
  class_destroy(dev_class);
  cdev_del(&select_test_cdev);
  unregister_chrdev_region(dev, 1);
  pr_info("Select Driver Remove...Done!!!\n");
}
 
module_init(select_test_driver_init);
module_exit(select_test_driver_exit);
 
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple linux driver (poll)");
MODULE_VERSION("1");
