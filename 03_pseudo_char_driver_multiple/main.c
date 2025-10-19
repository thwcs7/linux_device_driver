#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>

#define NO_OF_DEVICES 4

#define MEM_SIZE_MAX_PCDEV1 1024
#define MEM_SIZE_MAX_PCDEV2 512
#define MEM_SIZE_MAX_PCDEV3 1024
#define MEM_SIZE_MAX_PCDEV4 512

#undef pr_fmt
#define pr_fmt(fmt) "%s :" fmt,__func__

/* pseudu device's memory */
char device_buffer_pcdev1[MEM_SIZE_MAX_PCDEV1];
char device_buffer_pcdev2[MEM_SIZE_MAX_PCDEV2];
char device_buffer_pcdev3[MEM_SIZE_MAX_PCDEV3];
char device_buffer_pcdev4[MEM_SIZE_MAX_PCDEV4];

/* Device private data structure */
struct pcdev_private_data
{
    char *buffer;
    unsigned size;
    const char *serial_number;
    int perm;
    /* Cdev variable*/
    struct cdev cdev;
};

#define RDONLY 0x01
#define WRONLY 0x10
#define RDWR   0x11

/* Driver private data structure */
struct pcdrv_private_data
{
    int total_devices;
    /* This holds the device number*/
    dev_t device_number;
    struct class *class_pcd;
    struct device *device_pcd;
    struct pcdev_private_data pcdev_data[NO_OF_DEVICES];
};

struct pcdrv_private_data pcdrv_data =
{
    .total_devices = NO_OF_DEVICES,
    .pcdev_data = {

        [0] = {
            .buffer = device_buffer_pcdev1,
            .size = MEM_SIZE_MAX_PCDEV1,
            .serial_number = "PCDEV1XYZ123",
            .perm = RDONLY /* RDONLY */
        },

        [1] = {
            .buffer = device_buffer_pcdev2,
            .size = MEM_SIZE_MAX_PCDEV2,
            .serial_number = "PCDEV2XYZ123",
            .perm = WRONLY /* WRONLY */            
        },

        [2] = {
            .buffer = device_buffer_pcdev3,
            .size = MEM_SIZE_MAX_PCDEV3,
            .serial_number = "PCDEV3XYZ123",
            .perm = RDWR /* WRONLY */            
        },

        [3] = {
            .buffer = device_buffer_pcdev4,
            .size = MEM_SIZE_MAX_PCDEV4,
            .serial_number = "PCDEV4XYZ123",
            .perm = RDWR /* WRONLY */            
        },

    }
};


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


loff_t pcd_lseek(struct file *filp, loff_t off, int whence)
{
    pr_info("lseek requested\n");
    return 0;
}

ssize_t pcd_read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos)
{
    struct pcdev_private_data *pcdev_data = (struct pcdev_private_data *)filp->private_data;

    int max_size = pcdev_data->size;

    pr_info("read requested for %zu byte\n", count);
    pr_info("Current file position = %lld\n", *f_pos);

    /* Adjust the 'count' */
    if((*f_pos + count) > max_size) {
        count = max_size - *f_pos;
    }

    /* copy to user */
    if(copy_to_user(buff, pcdev_data->buffer + (*f_pos), count)) {
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
    struct pcdev_private_data *pcdev_data = (struct pcdev_private_data *)filp->private_data;

    int max_size = pcdev_data->size;

    pr_info("write requested for %zu byte\n", count);
    pr_info("Current file position = %lld\n", *f_pos);

    /* Adjust the 'count' */
    if((*f_pos + count) > max_size) {
        count = max_size - *f_pos;
    }

    if(!count) {
        return -ENOMEM;
    }

    /* copy from user */
    if(copy_from_user(pcdev_data->buffer + (*f_pos), buff, count)) {
        return -EFAULT;
    }

    /* update the current file position */
    *f_pos += count;

    pr_info("Number of bytes successfully written = %zu\n", count);
    pr_info("Update file position = %lld\n", *f_pos);

    /* Return number of bytes which have been successfully write */
    return count;
}

static int check_permission(int dev_per, int acc_mode)
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

int pcd_open(struct inode *inode, struct file *filp)
{
    int ret;

    int minor_n;

    struct pcdev_private_data *pcdev_data;

    /* find out on which device file open was attempted by the user space */
    minor_n = MINOR(inode->i_rdev);
    pr_info("minor access = %d\n", minor_n);
    /* get device's private data structure */
    pcdev_data = container_of(inode->i_cdev, struct pcdev_private_data, cdev);

    /* to supply device private data to other methods of driver */
    filp->private_data = pcdev_data;

    /* check permission */
    ret = check_permission(pcdev_data->perm, filp->f_mode);

    (!ret)?pr_info("open was successful\n"):pr_info("open was unsuccessful\n");

    return ret;
}

int pcd_release(struct inode *inode, struct file *filp)
{
    pr_info("close was successful\n");
    return 0;
}

static int __init pcd_driver_int(void)
{

    int ret;
    int i;

    /* 1. Dynamically allocate a device number*/
    ret = alloc_chrdev_region(&pcdrv_data.device_number, 0, NO_OF_DEVICES, "pcd_devices");
    if(ret < 0) 
        goto unalloc_chrdev;

    /* 2. Create device class under /sys/class */
    pcdrv_data.class_pcd = class_create("pcd_class");
    if(IS_ERR(pcdrv_data.class_pcd)) {
        pr_err("Class create failed\n");
        ret = PTR_ERR(pcdrv_data.class_pcd);
        goto unreg_chrdev;
    }
    
    for(i = 0; i < NO_OF_DEVICES; i++) {
        pr_info("Device number <major>:<minor> = %d:%d\n", MAJOR(pcdrv_data.device_number+i), MINOR(pcdrv_data.device_number+i)); 

        /* 3. Initialize the cdev structure with fops */
        cdev_init(&pcdrv_data.pcdev_data[i].cdev, &pcd_fops);

        /* 4. Register a device (cdev) structure with VFS */
        pcdrv_data.pcdev_data[i].cdev.owner = THIS_MODULE;
        ret = cdev_add(&pcdrv_data.pcdev_data[i].cdev, pcdrv_data.device_number+i, 1);
        if(ret < 0)
            goto cdev_del;

        /* 5. Populate the sysfs with device information */
        pcdrv_data.device_pcd = device_create(pcdrv_data.class_pcd, NULL, pcdrv_data.device_number+i, NULL, "pcdev-%d", i);
        if(IS_ERR(pcdrv_data.device_pcd)) {
            pr_err("Device create failed\n");
            ret = PTR_ERR(pcdrv_data.device_pcd);
            goto class_del;
        }
    }
    pr_info("Module init was successful\n");

    return 0;

cdev_del:
    cdev_del(&pcdrv_data.pcdev_data[i].cdev);
class_del:
    for(;i >= 0; i--) {
        device_destroy(pcdrv_data.class_pcd, pcdrv_data.device_number+i);
        cdev_del(&pcdrv_data.pcdev_data[i].cdev);
    }
    class_destroy(pcdrv_data.class_pcd);
unreg_chrdev:
    unregister_chrdev_region(pcdrv_data.device_number, NO_OF_DEVICES);
unalloc_chrdev:
    return ret;
}

static void __exit pcd_driver_exit(void)
{
    int i;

    for(i = 0;i < NO_OF_DEVICES; i++) {
        device_destroy(pcdrv_data.class_pcd, pcdrv_data.device_number+i);
        cdev_del(&pcdrv_data.pcdev_data[i].cdev);
    }

    class_destroy(pcdrv_data.class_pcd);

    unregister_chrdev_region(pcdrv_data.device_number, NO_OF_DEVICES);

    pr_info("module unloaded\n");
}

module_init(pcd_driver_int);
module_exit(pcd_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ngo Xuan Thuc");
MODULE_DESCRIPTION("A simple pseudo character driver which handle n devices");