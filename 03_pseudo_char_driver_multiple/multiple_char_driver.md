# Character driver with multiple device node

### In the driver, we will maintain two structures
- Structure which holds driver's private data.
- Structure which holds device's private data.

### Structure to represent device's private data
````c
struct pcdev_private_data
{
    char *buffer;
    unsigned size;
    const char *serial_number;
    int perm;
    /* Cdev variable*/
    struct cdev cdev;
};
````

### Structure to represent driver's private data
````c
struct pcdrv_private_data
{
    int total_devices;
    /* This holds the device number*/
    dev_t device_number;
    struct class *class_pcd;
    struct device *device_pcd;
    struct pcdev_private_data pcdev_data[NO_OF_DEVICES];
};
````

### container_of
+ container_of macro helps you to get the address of the containing structure by talking an address of its member element. 
+ As its name indicates it gives you the “container” address of the member element of a struct.
+ It takes three arguments – a pointer, type of the container, and the name of the member the pointer refers to.

````c
#define container_of(ptr, type, member)
````
| Parameter | Description |
|----------|-------|
| **ptr** | The pointer to the member. |
| **type** | The type of the container struct this is embedded in. |
| **member** | The name of the member the `ptr` within the struct. |

