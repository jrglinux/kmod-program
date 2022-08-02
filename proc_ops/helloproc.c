#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
#define HAVE_PROC_OPS
#endif

#define procfs_name     "helloproc"
static struct proc_dir_entry *hello_proc_file;

static ssize_t procfile_read(struct file *filePointer, char __user *buffer,
				size_t buf_len, loff_t *offset)
{
    char s[11] = "helloproc!\n";
    int len = sizeof(s);
    ssize_t ret =len;

    if(*offset >= len || copy_to_user(buffer, s, len)){
    	pr_info("copy_to_user failed\n");
	ret = 0;
    }else{
    	pr_info("procfile read %s\n", filePointer->f_path.dentry->d_name.name);
	*offset +=len;
    }
    return ret;
}

#ifdef HAVE_PROC_OPS
static const struct proc_ops proc_file_ops = {
    .proc_read = procfile_read,
};
#else
static const struct file_operations proc_file_ops = {
    .read = procfile_read,
};
#endif

static int __init helloproc_init(void)
{
    hello_proc_file = proc_create(procfs_name, 0644, NULL, &proc_file_ops);
    if (NULL == hello_proc_file) {
        proc_remove(hello_proc_file);
        pr_alert("can not create %s\n", procfs_name);
        return -ENOMEM;
    }
    pr_info("/proc/%s created\n", procfs_name);
    return 0;
}

static void __exit helloproc_exit(void)
{
    proc_remove(hello_proc_file);
    pr_info("/proc/%s has been removed\n", procfs_name);
}

module_init(helloproc_init);
module_exit(helloproc_exit);
MODULE_LICENSE("GPL");
