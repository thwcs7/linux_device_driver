#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/mod_devicetable.h>

#include "platform.h"

#undef pr_fmt
#define pr_fmt(fmt) "%s :" fmt,__func__

struct device_config
{
    int config_item1;
    int config_item2;
};

enum pcdev_names
{
    PCDEVA1X,
    PCDEVB1X
};

struct device_config pcdev_config[] =
{
    [PCDEVA1X] = {.config_item1 = 15, .config_item2 = 07},
    [PCDEVB1X] = {.config_item1 = 03, .config_item2 = 11},
};

/* Device private data structure */
struct pcdev_private_data
{
    struct pcdev_platform_data pdata;
    char *buffer;
    dev_t dev_num;
    /* Cdev variable*/
    struct cdev cdev;
};

/* Driver private data structure */
struct pcdrv_private_data
{
    int total_devices;
    /* This holds the device number*/
    dev_t device_num_base;
    struct class *class_pcd;
    struct device *device_pcd;
};

struct pcdrv_private_data pcdrv_data;

/*  Function Prototypes */
static int  __init pcd_platform_driver_init(void);
static void __exit pcd_platform_driver_exit(void);
loff_t  pcd_lseek(struct file *filp, loff_t off, int whence);
ssize_t pcd_read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos);
ssize_t pcd_write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos);
int     pcd_open(struct inode *inode, struct file *filp);
int     pcd_release(struct inode *inode, struct file *filp);

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

struct pcdev_platform_data* pcdev_get_platdata_from_dt(struct device *dev)
{
    struct device_node *dev_node = dev->of_node;
    struct pcdev_platform_data *pdata;

    if(!dev_node)
        /* This probe didn't happen because of device node */
        return NULL;

    pdata = devm_kzalloc(dev, sizeof(*pdata), GFP_KERNEL);
    if(!pdata) {
        dev_info(dev, "Cannot allocate memory \n");
        return ERR_PTR(-ENOMEM);
    }

    if(of_property_read_string(dev_node, "org,device-serial-num", &pdata->serial_number)) {
        dev_info(dev, "Missing serial number property\n");
        return ERR_PTR(-EINVAL);
    }

    if(of_property_read_u32(dev_node, "org,size", &pdata->size)) {
        dev_info(dev, "Missing size property\n");
        return ERR_PTR(-EINVAL);
    }

    if(of_property_read_u32(dev_node, "org,perm", &pdata->perm)) {
        dev_info(dev, "Missing permission property\n");
        return ERR_PTR(-EINVAL);
    }

    return pdata;
}

static int pcd_platform_driver_probe(struct platform_device *pdev)
{
    int ret;

    struct pcdev_private_data *dev_data;

    struct pcdev_platform_data *pdata;

    // struct of_device_id *match;

    struct device *dev = &pdev->dev;

    int driver_data;

    dev_info(dev, "A device is detected\n");

    pdata = pcdev_get_platdata_from_dt(dev);

    if(IS_ERR(pdata))
        return -EINVAL;
        // return PTR_ERR(pdata);
    if(!pdata)
    {
        /* 1. Get the platform data */
        pdata = (struct pcdev_platform_data *)dev_get_platdata(dev);
        if(!pdata) {
            dev_info(dev, "No platform data available\n");
            ret = -EINVAL;
            goto out;
        }

        driver_data = pdev->id_entry->driver_data;
    }
    else
    {
        driver_data = (int)of_device_get_match_data(dev);
        // match = of_match_device(pdev->dev.driver->of_match_table, &pdev->dev);
        // driver_data = (int)match->data;
    }

    /* 2. Dynamically allocate memory for the device private data */
    dev_data = devm_kzalloc(&pdev->dev, sizeof(*dev_data), GFP_KERNEL);
    if(!dev_data) {
        dev_info(dev, "Can not allocate memory\n");
        ret = -ENOMEM;
        goto out;
    }

    /* Save the device private data pointer in platform device structure */
    dev_set_drvdata(&pdev->dev, dev_data);

    dev_data->pdata.size = pdata->size;
    dev_data->pdata.perm = pdata->perm;
    dev_data->pdata.serial_number = pdata->serial_number;

    dev_info(dev, "Device serial number = %s\n", dev_data->pdata.serial_number);
    dev_info(dev, "Device_size = %d\n", dev_data->pdata.size);
    dev_info(dev, "Device permission = %d\n", dev_data->pdata.perm);

    dev_info(dev, "Config item 1 = %d\n",
            pcdev_config[driver_data].config_item1);

    dev_info(dev, "Config item 2 = %d\n",
            pcdev_config[driver_data].config_item2);


    /* 3. Dynamically allocate memory for the device buffer using size
    information from the flatform data */
    dev_data->buffer = devm_kzalloc(&pdev->dev, dev_data->pdata.size, GFP_KERNEL);
    if(!dev_data->buffer) {
        dev_info(dev, "Can not allocate memory\n");
        ret = -ENOMEM;
        goto dev_data_free;
    }
    /* 4. Get the device number */
    dev_data->dev_num = pcdrv_data.device_num_base + pcdrv_data.total_devices;
    /* 5. Do cdev init and cdev add */
    cdev_init(&dev_data->cdev, &pcd_fops);

    dev_data->cdev.owner = THIS_MODULE;
    ret = cdev_add(&dev_data->cdev, dev_data->dev_num, 1);
    if(ret < 0) {
        goto buffer_free;
    }
    /* 6. Create device file for the detected platform device */
    pcdrv_data.device_pcd = device_create(pcdrv_data.class_pcd, dev, dev_data->dev_num, NULL, "pcdev-%d", pcdrv_data.total_devices);
    if(IS_ERR(pcdrv_data.device_pcd)) {
        dev_err(dev, "Device create failed\n");
        ret = PTR_ERR(pcdrv_data.device_pcd);
        goto cdev_del;
    }

    pcdrv_data.total_devices++;

    /* 7. Error handling */

    dev_info(dev, "The probe was successful\n");

    return 0;

cdev_del:
    cdev_del(&dev_data->cdev);
buffer_free:
    devm_kfree(&pdev->dev, dev_data->buffer);
dev_data_free:
    devm_kfree(&pdev->dev, dev_data);
out:
    dev_info(dev, "Device probe failed \n");
    return ret;
}

static int pcd_platform_driver_remove(struct platform_device *pdev)
{
    struct pcdev_private_data *dev_data = dev_get_drvdata(&pdev->dev);

    /* 1. Remove a device that was created with device_create() */
    device_destroy(pcdrv_data.class_pcd, dev_data->dev_num);

    /* 2. Remove a cdev entry from the system */
    cdev_del(&dev_data->cdev);

    /* 3. Free the memory held by the device */
    // kfree(dev_data->buffer);
    // kfree(dev_data);

    pcdrv_data.total_devices--;

    dev_info(&pdev->dev, "A device is removed \n");

    return 0;
}

struct platform_device_id pcdevs_ids[] =
{
    {.name = "pcdev-A1x", .driver_data = PCDEVA1X},
    {.name = "pcdev-B1x", .driver_data = PCDEVB1X},

    {   }
};

struct of_device_id org_pcdev_dt_match[]  = 
{
    {.compatible = "pcdev-A1x", .data = (void*)PCDEVA1X},
    {.compatible = "pcdev-B1x", .data = (void*)PCDEVB1X},

    {   }
};

struct platform_driver pcd_platform_driver =
{
    .probe  = pcd_platform_driver_probe,
    .remove = pcd_platform_driver_remove,
    .id_table = pcdevs_ids,
    .driver = {
        .name = "pseudo-char-device",
        .of_match_table = org_pcdev_dt_match
    }
};

//https://elixir.bootlin.com/linux/v6.17/source/drivers/base/platform.c

#define MAX_DEVICES 10

static int __init pcd_platform_driver_init(void)
{
    int ret;

    /* 1. Dynamically allocate a device number for MAX_DEVICES */
    ret = alloc_chrdev_region(&pcdrv_data.device_num_base, 0, MAX_DEVICES, "pcdevs");
    if(ret < 0) {
        pr_err("Alloc chrdev failed\n");
        return ret;
    }

    /* 2. Create device class under /sys/class */
    pcdrv_data.class_pcd = class_create(THIS_MODULE,"pcd_class");
    if(IS_ERR(pcdrv_data.class_pcd)) {
        pr_err("Class create failed\n");
        ret = PTR_ERR(pcdrv_data.class_pcd);
        unregister_chrdev_region(pcdrv_data.device_num_base, MAX_DEVICES);
        return ret;
    }

    /* 3. Register a platform driver */
    platform_driver_register(&pcd_platform_driver);
    pr_info("pcd platform driver loaded \n");
    return 0;
}

static void __exit pcd_platform_driver_exit(void)
{
    /* 1. Unregister the platform driver */
    platform_driver_unregister(&pcd_platform_driver);

    /* 2. Class destroy */
    class_destroy(pcdrv_data.class_pcd);

    /* 3. Unregister device numbers for MAX_DEVICES*/
    unregister_chrdev_region(pcdrv_data.device_num_base, MAX_DEVICES);

    pr_info("pcd platform driver unloaded \n");
}

module_init(pcd_platform_driver_init);
module_exit(pcd_platform_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ngo Xuan Thuc");
MODULE_DESCRIPTION("A pseudo character platform driver which handle n platform pcdevs");