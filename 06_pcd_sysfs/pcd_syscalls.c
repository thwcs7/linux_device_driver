#include "pcd_platform_driver_dt_sysfs.h"

int check_permission(int dev_per, int acc_mode)
{
    if(dev_per == RDWR)
        return 0;

    //ensures readonly access
    if((dev_per == RDONLY) && ((acc_mode & FMODE_READ) && !(acc_mode & FMODE_WRITE)))
        return 0;

    //ensures writeonly access
    if((dev_per == WRONLY) && ((acc_mode & FMODE_WRITE) && !(acc_mode & FMODE_READ)))
        return 0;

    return -EPERM;
}


loff_t pcd_lseek(struct file *filp, loff_t off, int whence)
{
    return 0;
}

ssize_t pcd_read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos)
{
    return 0;
}

ssize_t pcd_write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos)
{
    return -ENOMEM;
}

int pcd_open(struct inode *inode, struct file *filp)
{
    return 0;
}

int pcd_release(struct inode *inode, struct file *filp)
{
    pr_info("close was successful\n");

    return 0;
}