#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include<linux/slab.h>                 //kmalloc()
#include<linux/uaccess.h>              //copy_to/from_user()
#include <linux/kthread.h>             //kernel threads
#include <linux/sched.h>               //task_struct 
#include <linux/delay.h>
#include <linux/mutex.h>
 
struct mutex test_mutex;
unsigned long test_mutex_global_variable = 0;
 
static int __init test_mutex_driver_init(void);
static void __exit test_mutex_driver_exit(void);
 
static struct task_struct *test_mutex_thread1;
static struct task_struct *test_mutex_thread2; 
 
int thread_function1(void *pv);
int thread_function2(void *pv);
/*
** Thread function 1
*/
int thread_function1(void *pv)
{
    
    while(!kthread_should_stop()) {
        mutex_lock(&test_mutex);
        test_mutex_global_variable++;
        pr_info("In Thread Function1 %lu\n", test_mutex_global_variable);
        mutex_unlock(&test_mutex);
        msleep(1000);
    }
    return 0;
}
/*
** Thread function 2
*/
int thread_function2(void *pv)
{
    while(!kthread_should_stop()) {
        mutex_lock(&test_mutex);
        test_mutex_global_variable++;
        pr_info("In Thread Function2 %lu\n", test_mutex_global_variable);
        mutex_unlock(&test_mutex);
        msleep(1000);
    }
    return 0;
}

/*
** Module Init function
*/
static int __init test_mutex_driver_init(void)
{
        mutex_init(&test_mutex);
        
        /* Creating Thread 1 */
        test_mutex_thread1 = kthread_run(thread_function1,NULL,"test Thread1");
        if(test_mutex_thread1) {
            pr_info("Kthread1 Created Successfully...\n");
        } else {
            pr_err("Cannot create kthread1\n");
	    return -1;
        }
 
         /* Creating Thread 2 */
        test_mutex_thread2 = kthread_run(thread_function2,NULL,"test Thread2");
        if(test_mutex_thread2) {
            pr_info("Kthread2 Created Successfully...\n");
        } else {
            pr_err("Cannot create kthread2\n");
	    return -1;
        }
        
        pr_info("test mutex driver Insert...Done!!!\n");
        return 0;
}

/*
** Module exit function
*/ 
static void __exit test_mutex_driver_exit(void)
{
        kthread_stop(test_mutex_thread1);
        kthread_stop(test_mutex_thread2);
        pr_info("test mutex driver Remove...Done!!\n");
}
 
module_init(test_mutex_driver_init);
module_exit(test_mutex_driver_exit);
 
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A simple kernel driver - Mutex");
