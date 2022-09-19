#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kthread.h>             //kernel threads
#include <linux/sched.h>               //task_struct 
#include <linux/delay.h>
#include <linux/seqlock.h>
 
//Seqlock variable
seqlock_t test_seq_lock;

unsigned long test_global_variable = 0;
 
static struct task_struct *test_thread1;
static struct task_struct *test_thread2;
 
//Thread used for writing
int thread_function1(void *pv)
{
    while(!kthread_should_stop()) {  
        write_seqlock(&test_seq_lock);
        test_global_variable++;
        pr_info("In Thread Function1 : write value %lu\n", test_global_variable);
        write_sequnlock(&test_seq_lock);
        msleep(1000);
    }
    return 0;
}
 
//Thread used for reading
int thread_function2(void *pv)
{
    unsigned int seq_no;
    unsigned long read_value;
    while(!kthread_should_stop()) {
        do {
            seq_no = read_seqbegin(&test_seq_lock);
        read_value = test_global_variable;
    } while (read_seqretry(&test_seq_lock, seq_no));
        pr_info("In Thread Function2 : Read value %lu\n", read_value);
        msleep(1000);
    }
    return 0;
}

static int __init seqlock_driver_init(void)
{
        /* Creating Thread 1 */
        test_thread1 = kthread_run(thread_function1,NULL,"seqlock Thread1");
        if(test_thread1) {
            pr_err("Kthread1 Created Successfully...\n");
        } else {
            pr_err("Cannot create kthread1\n");
            return -1;
        }
 
         /* Creating Thread 2 */
        test_thread2 = kthread_run(thread_function2,NULL,"seqlock Thread2");
        if(test_thread2) {
            pr_err("Kthread2 Created Successfully...\n");
        } else {
            pr_err("Cannot create kthread2\n");
            return -1;
        }
 
        //Initialize the seqlock
        seqlock_init(&test_seq_lock);
        
        pr_info("Seqlock Driver Insert...Done!!!\n");
        return 0;
}
/*
** Module exit function
*/
static void __exit seqlock_driver_exit(void)
{
        kthread_stop(test_thread1);
        kthread_stop(test_thread2);
        pr_info("Seqlock Driver Remove...Done!!\n");
}
 
module_init(seqlock_driver_init);
module_exit(seqlock_driver_exit);
 
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A simple device driver - Seqlock");
MODULE_VERSION("1.28");
