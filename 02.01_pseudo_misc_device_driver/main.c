#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>



#define DEV_MEM_SIZE 512

#undef pr_fmt
#define pr_fmt(fmt) "%s :" fmt,__func__

/* pseudu device's memory */
char device_buffer[DEV_MEM_SIZE];

/* This holds the device number*/
dev_t device_number;

/* Cdev variable*/
struct cdev pcd_cdev;

/*  Function Prototypes */
static int  __init pcd_driver_int(void);
static void __exit pcd_driver_exit(void);
loff_t  pcd_lseek(struct file *filp, loff_t off, int whence);
ssize_t pcd_read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos);
ssize_t pcd_write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos);
int     pcd_open(struct inode *inode, struct file *filp);
int     pcd_release(struct inode *inode, struct file *filp);

/* File operation of the driver */
struct file_operations pcd_fops =
{
    .owner   = THIS_MODULE,
    .open    = pcd_open,
    .write   = pcd_write,
    .read    = pcd_read,
    .llseek  = pcd_lseek,
    .release = pcd_release,
};

static struct miscdevice device_pcd = {
    .name = "pcd",
    .minor = MISC_DYNAMIC_MINOR,
    .fops = &pcd_fops,
};


loff_t pcd_lseek(struct file *filp, loff_t off, int whence)
{
    pr_info("lseek requested\n");
    return 0;
}

ssize_t pcd_read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos)
{
    pr_info("read requested for %zu byte\n", count);
    pr_info("Current file position = %lld\n", *f_pos);

    /* Adjust the 'count' */
    if((*f_pos + count) > DEV_MEM_SIZE) {
        count = DEV_MEM_SIZE - *f_pos;
    }

    /* copy to user */
    if(copy_to_user(buff, &device_buffer[*f_pos], count)) {
        return -EFAULT;
    }

    /* update the current file position */
    *f_pos += count;

    pr_info("Number of bytes successfully read = %zu\n", count);
    pr_info("Update file position = %lld\n", *f_pos);

    /* Return number of bytes which have been successfully read */
    return count;
}

ssize_t pcd_write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos)
{
    pr_info("write requested for %zu byte\n", count);
    pr_info("Current file position = %lld\n", *f_pos);

    /* Adjust the 'count' */
    if((*f_pos + count) > DEV_MEM_SIZE) {
        count = DEV_MEM_SIZE - *f_pos;
    }

    if(!count) {
        return -ENOMEM;
    }

    /* copy from user */
    if(copy_from_user(&device_buffer[*f_pos], buff, count)) {
        return -EFAULT;
    }

    /* update the current file position */
    *f_pos += count;

    pr_info("Number of bytes successfully written = %zu\n", count);
    pr_info("Update file position = %lld\n", *f_pos);

    /* Return number of bytes which have been successfully write */
    return count;
}

int pcd_open(struct inode *inode, struct file *filp)
{
    pr_info("open was successful\n");
    return 0;
}

int pcd_release(struct inode *inode, struct file *filp)
{
    pr_info("close was successful\n");
    return 0;
}

static int __init pcd_driver_int(void)
{
    int ret;

    /* 1. Register a misc device */
    ret = misc_register(&device_pcd);

    pr_info("Misc device number <major>:<minor> = %d:%d\n", MISC_MAJOR, device_pcd.minor);

    if(ret) {
        pr_info("Error during register misc device\n");
    }

    pr_info("Misc device init was successful\n");

    return 0;
}

static void __exit pcd_driver_exit(void)
{

    misc_deregister(&device_pcd);
    pr_info("Misc device unloaded\n");
}

module_init(pcd_driver_int);
module_exit(pcd_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ngo Xuan Thuc");
MODULE_DESCRIPTION("A simple pseudo character driver");