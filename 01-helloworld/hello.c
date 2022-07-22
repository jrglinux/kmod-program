#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/stat.h>

static short int dshort = 100;
static int dint = 100;
static long int dlong = 12345678;
static char *dstring = "linux";
static int dints[4] = { 100, 100, 100, 100 };
static int arr_num = 0;

module_param(dshort, short, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(dshort, "A short integer");

module_param(dint, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(dint, "An integer");

module_param(dlong, long, S_IRUSR);
MODULE_PARM_DESC(dlong, "A long integer");

module_param(dstring, charp, 0000);
MODULE_PARM_DESC(dstring, "A character string");

module_param_array(dints, int, &arr_num, 0000);
MODULE_PARM_DESC(dints, "An array of integers");

static int __init hello_kmod_init(void)
{
    int i;
    
    pr_info("Hello kmod\n");
    pr_info("dshort is a short integer: %hd\n", dshort);
    pr_info("dint is an integer: %d\n", dint);
    pr_info("dlong is a long integer: %ld\n", dlong);
    pr_info("dstring is a string: %s\n", dstring);
    
    for (i = 0; i < ARRAY_SIZE(dints); i++)
        pr_info("dints[%d] = %d\n", i, dints[i]);
    
    pr_info("got %d arguments for dints.\n", arr_num);
    return 0;
}

static void __exit hello_kmod_exit(void)
{
    pr_info("Goodbye, hello kmod\n");
}
module_init(hello_kmod_init);
module_exit(hello_kmod_exit);
MODULE_LICENSE("GPL");
