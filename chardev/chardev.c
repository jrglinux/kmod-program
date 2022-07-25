#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/irq.h>
#include <linux/kernel.h>
#include <linux/poll.h>

static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char __user *, size_t, loff_t *);

#define DEVICE_NAME "chardev"
#define BUF_LEN	80

static char msg[BUF_LEN];

static int major;	/*major number assigned to our device driver*/

enum {
    CDEV_NOT_USED = 0,
    CDEV_IN_USED = 1,
};
static atomic_t in_use = ATOMIC_INIT(CDEV_NOT_USED); /*is it opened? used to prevent multiple access to device*/

static struct class *cls;

static struct file_operations chardev_fops = {
    .read = dev_read,
    .write = dev_write,
    .open = dev_open,
    .release = dev_release,
};

static int __init chardev_init(void){
    major = register_chrdev(0, DEVICE_NAME, &chardev_fops);	
    if(major < 0){
        pr_alert("failed to register_chrdev with %d\n", major);
        return major;
    }

    pr_info("I was assigned major number %d\n", major);

    cls = class_create(THIS_MODULE, DEVICE_NAME);
    device_create(cls, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);

    pr_info("Device created on /dev/%s\n", DEVICE_NAME);
    return 0;
}

static void __exit chardev_exit(void){
    device_destroy(cls, MKDEV(major, 0));
    class_destroy(cls);

    unregister_chrdev(major, DEVICE_NAME);
}

static int dev_open(struct inode *inode, struct file *file)
{
    static int counter = 0;
    if(atomic_cmpxchg(&in_use, CDEV_NOT_USED, CDEV_IN_USED))
        return -EBUSY;

    sprintf(msg, "I already told you %d times hello\n", counter++);
    try_module_get(THIS_MODULE);

    return 0;
}

static int dev_release(struct inode *inode, struct file *file)
{
    atomic_set(&in_use, CDEV_NOT_USED);

    module_put(THIS_MODULE);
    return 0;
}

static ssize_t dev_read(struct file *filp, 
                        char __user *buffer,
                        size_t length,
                        loff_t *offset)
{
    int bytes_read = 0;
    const char *msg_ptr = msg;

    /*if we are at the end of msg, reset the offset*/
    if(!*(msg_ptr + *offset)){
        *offset = 0;
        return 0;
    }

    msg_ptr += *offset;

    /*the buffer is in the userspace, so we use put_user() copy data
     * from kernel to user data segment.
     */
    while(length && *msg_ptr){
        put_user(*(msg_ptr++), buffer++);
        length--;
        bytes_read++;
    }

    *offset += bytes_read;

    return bytes_read;
}

static ssize_t dev_write(struct file *filp, const char __user *buffer,
                        size_t len, loff_t *off)
{
    pr_alert("Sorry, this operation is not supported\n");
    return -EINVAL;
}

module_init(chardev_init);
module_exit(chardev_exit);
MODULE_LICENSE("GPL");
