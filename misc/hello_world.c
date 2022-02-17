/*
 * Jonathan Corbet, Alessandro Rubini, and Greg Kroah-Hartman. 2005. 
 * <i>Linux Device Drivers, 3rd Edition</i>. O'Reilly Media, Inc.
 */
#include <linux/init.h>
#include <linux/module.h>
MODULE_LICENSE("Dual BSD/GPL");

static int hello_init(void)
{
    printk(KERN_ALERT "Hello, world\n");
    return 0;
}

static void hello_exit(void)
{
    printk(KERN_ALERT "Goodbye, cruel world\n");
}

module_init(hello_init);
module_exit(hello_exit);
