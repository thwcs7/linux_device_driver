#include <linux/module.h>
#include <linux/platform_device.h>

#include "platform.h"

#undef pr_fmt
#define pr_fmt(fmt) "%s :" fmt,__func__

static void pcdev_release(struct device *dev)
{
    pr_info("Device released \n");
}

//1. Create 2 platform data
struct pcdev_platform_data pcdev_pdata[2] =
{
    [0] = {.size = 512, .perm = RDWR, .serial_number = "PCDEVABC15072003"},
    [1] = {.size = 1024,.perm = RDWR, .serial_number = "PCDEVABC03112003"}
};
//2. Create 2 platform devices

struct platform_device platform_pcdev_1 = 
{
    .name = "pcdev-A1x",
    .id = 0,
    .dev = {
            .platform_data = &pcdev_pdata[0],
            .release = pcdev_release
    }
};

struct platform_device platform_pcdev_2 = 
{
    .name = "pcdev-B1x",
    .id = 1,
    .dev = {
            .platform_data = &pcdev_pdata[1],
            .release = pcdev_release
    }
};

struct platform_device *platform_pcdevs[] =
{
    &platform_pcdev_1,
    &platform_pcdev_2
};

static int __init pcdev_platform_init(void)
{
    //register platform device
    // platform_device_register(&platform_pcdev_1);
    // platform_device_register(&platform_pcdev_2);

    platform_add_devices(platform_pcdevs, ARRAY_SIZE(platform_pcdevs));

    pr_info("Device setup module loaded \n");

    return 0;
}

static void __exit pcdev_platform_exit(void)
{
    platform_device_unregister(&platform_pcdev_1);
    platform_device_unregister(&platform_pcdev_2); 
    
    pr_info("Device setup module unloaded \n");
}

module_init(pcdev_platform_init);
module_exit(pcdev_platform_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ngo Xuan Thuc");
MODULE_DESCRIPTION("A simple pseudo platform devices");
