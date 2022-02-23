/*
 * main.c -- the bare scull char module
 *
 * Copyright (C) 2001 Alessandro Rubini and Jonathan Corbet
 * Copyright (C) 2001 O'Reilly & Associates
 *
 * 
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files.  The citation
 * should list that the code comes from the book "Linux Device
 * Drivers" by Alessandro Rubini and Jonathan Corbet, published
 * by O'Reilly & Associates.   No warranty is attached;
 * we cannot take responsibility for errors or fitness for use.
 *
 */

#include <linux/module.h>
#include <linux/init.h>

#include <linux/cdev.h>
#include <linux/fs.h>    /* everything... */

#include "scull.h"

/*
 * Our parameters which can be set at load time.
 */

int scull_major =   SCULL_MAJOR;
int scull_minor =   0;
int scull_nr_devs = SCULL_NR_DEVS;    /* number of bare scull devices */
int scull_quantum = SCULL_QUANTUM;
int scull_qset =    SCULL_QSET;

MODULE_AUTHOR("Alessandro Rubini, Jonathan Corbet");
MODULE_LICENSE("Dual BSD/GPL");

struct scull_dev *scull_devs;         /* allocated in scull_init_module */

struct file_operations scull_fops = {
    .owner =    THIS_MODULE,
    /* .llseek =   scull_llseek, */
    /* .read =     scull_read, */
    /* .write =    scull_write, */
    /* .ioctl =    scull_ioctl, */
    /* .open =     scull_open, */
    /* .release =  scull_release, */
};


/*
 * The cleanup function is used to handle initialization failures as well.
 * Thefore, it must be careful to work correctly even if some of the items
 * have not been initialized
 */
void scull_cleanup_module(void)
{
    int i;
    dev_t devno = MKDEV(scull_major, scull_minor);

    /* cleanup_module is never called if registering failed */
    unregister_chrdev_region(devno, scull_nr_devs);

    printk(KERN_ALERT "bye scull!\n");
}

/*
 * Set up the char_dev structure for this device.
 */
static void scull_setup_cdev(struct scull_dev *dev, int index)
{
    int err, devno = MKDEV(scull_major, scull_minor + index);
    
    cdev_init(&dev->cdev, &scull_fops);
    dev->cdev.owner = THIS_MODULE;
    dev->cdev.ops = &scull_fops;
    err = cdev_add (&dev->cdev, devno, 1);
    /* Fail gracefully if need be */
    if (err)
	printk(KERN_NOTICE "Error %d adding scull%d", err, index);
}

int scull_init_module(void)
{
    int res, i;
    dev_t dev = 0;

    /*
     * Get a range of minor numbers to work with, asking for a dynamic
     * major unless directed otherwise at load time.
     */
    if (scull_major) {
	dev = MKDEV(scull_major, scull_minor);
	res = register_chrdev_region(dev, scull_nr_devs, "scull");
    } else {
	res = alloc_chrdev_region(&dev, scull_minor, scull_nr_devs, "scull");
	scull_major = MAJOR(dev);
    }
    if (res < 0) {
	printk(KERN_WARNING "scull: can't get major %d\n", scull_major);
	return res;
    }
    printk(KERN_ALERT "scull: major %d - minor %d\n", scull_major, scull_minor);

    /* 
     * allocate the devices -- we can't have them static, as the number
     * can be specified at load time
     */
    scull_devs = kmalloc(scull_nr_devs * sizeof(struct scull_dev), GFP_KERNEL);
    if (!scull_devs) {
	res = -ENOMEM;
	goto fail;  /* Make this more graceful */
    }
    memset(scull_devs, 0, scull_nr_devs * sizeof(struct scull_dev));

    /* Initialize each device. */
    for (i = 0; i < scull_nr_devs; i++) {
	scull_devs[i].quantum = scull_quantum;
	scull_devs[i].qset = scull_qset;
	init_MUTEX(&scull_devs[i].sem);
	scull_setup_cdev(&scull_devs[i], i);
    }
    
    return 0; /* succeed */

  fail:
    scull_cleanup_module();
    return res;
}

module_init(scull_init_module);
module_exit(scull_cleanup_module);
