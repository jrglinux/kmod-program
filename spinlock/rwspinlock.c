#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include<linux/slab.h>                 //kmalloc()
#include<linux/uaccess.h>              //copy_to/from_user()
#include <linux/kthread.h>             //kernel threads
#include <linux/sched.h>               //task_struct 
#include <linux/delay.h>
 
 
//Static method to initialize the read write spinlock
static DEFINE_RWLOCK(test_rwlock); 
 
//Dynamic method to initialize the read write spinlock
//rwlock_t test_rwlock;
 
unsigned long test_rwlock_global_variable = 0;
 
static struct task_struct *test_rwlock_thread1;
static struct task_struct *test_rwlock_thread2; 
static struct task_struct *test_rwlock_thread3; 
 
/*
** thread function 1 write
*/
int thread_function1(void *pv)
{
    while(!kthread_should_stop()) {  
        write_lock(&test_rwlock);
        test_rwlock_global_variable++;
        pr_info("In Thread Function1 : write once\n");
        write_unlock(&test_rwlock);
        msleep(1000);
    }
    return 0;
}

/*
** thread function 2 - read
*/
int thread_function2(void *pv)
{
    while(!kthread_should_stop()) {
        read_lock(&test_rwlock);
        pr_info("In Thread Function2 : Read value %lu\n", test_rwlock_global_variable);
        read_unlock(&test_rwlock);
        msleep(1000);
    }
    return 0;
}

/*
** thread function 3 - read
*/
int thread_function3(void *pv)
{
    while(!kthread_should_stop()) {
        read_lock(&test_rwlock);
        pr_info("In Thread Function3 : Read value %lu\n", test_rwlock_global_variable);
        read_unlock(&test_rwlock);
        msleep(1000);
    }
    return 0;
}

/*
** Module Init function
*/ 
static int __init test_rwlock_driver_init(void)
{
        /* Creating Thread 1 */
        test_rwlock_thread1 = kthread_run(thread_function1,NULL,"test rwlock Thread1");
        if(test_rwlock_thread1) {
            pr_err("Kthread1 Created Successfully...\n");
        } else {
            pr_err("Cannot create kthread1\n");
      	    return -1;
        }
 
         /* Creating Thread 2 */
        test_rwlock_thread2 = kthread_run(thread_function2,NULL,"test rwlock Thread2");
        if(test_rwlock_thread2) {
            pr_err("Kthread2 Created Successfully...\n");
        } else {
            pr_err("Cannot create kthread2\n");
      	    return -1;
        }

        /* Creating Thread 3 */
        test_rwlock_thread3 = kthread_run(thread_function3,NULL,"test rwlock Thread3");
        if(test_rwlock_thread3) {
            pr_err("Kthread3 Created Successfully...\n");
        } else {
            pr_err("Cannot create kthread3\n");
      	    return -1;
        }
 
        //Dynamic method to initialize the read write spinlock
        //rwlock_init(&test_rwlock);
        
        pr_info("RWspinlock Driver Insert...Done!!!\n");
        return 0;
 
 
}
/*
** Module exit function
*/
static void __exit test_rwlock_driver_exit(void)
{
        kthread_stop(test_rwlock_thread1);
        kthread_stop(test_rwlock_thread2);
        kthread_stop(test_rwlock_thread3);
        pr_info("RWspinlock Driver Remove...Done!!\n");
}
 
module_init(test_rwlock_driver_init);
module_exit(test_rwlock_driver_exit);
 
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A simple device driver - RW Spinlock");
