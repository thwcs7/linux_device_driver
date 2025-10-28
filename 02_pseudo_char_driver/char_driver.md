# Character device and driver

## Character device registration
__Kernel APIs and utilities to be used in driver code__

| Kernel API | Purpose |
|-------------|----------|
| **alloc_chrdev_region()**, **register_chrdev_region()** | Create device number `/proc/devices` |
| **cdev_init()**, **cdev_add()** | Register the character device with the VFS |
| **class_create()**, **device_create()** | Create device files in `/dev` |

## Dynamic allocating char device number
```c
int alloc_chrdev_region(dev_t *dev, unsigned baseminor, unsigned count, const char *name);
````
| Parameter | Description |
|----------|-------|
| **dev** | Output parameter, stores the first assigned device number. |
| **baseminor** | First of the requested range of minor numbers. |
| **count** | Number of minor numbers required. |
| **name** | Name of the associated device or driver to appear in `/proc/devices`. |

## Initialize a cdev structure
```c
void cdev_init(struct cdev *cdev, const struct file_operations *fops);
````
| Parameter | Description |
|----------|-------|
| **cdev** | The structure to initialize. |
| **fops** | The file operations for this device. |

```c
struct cdev {
	struct kobject kobj;
	struct module *owner;
	const struct file_operations *ops;
	struct list_head list;
	dev_t dev;
	unsigned int count;
} __randomize_layout;
````

The `ops` member of `struct cdev` is a pointer to the driver's `file_operations` structure of the driver

## Add a char device (cdev structure) to the kernel VFS
```c
int cdev_add(struct cdev *p, dev_t dev, unsigned count)
````
| Parameter | Description |
|----------|-------|
| **p** | The cdev structure for the device. |
| **dev** | The first device number for which this device is responsible. |
| **count** | The number of consecutive minor numbers corresponding to this device. |

## Character driver file operation method
**VFS file operation structure**

```c
struct file_operations {
    struct module *owner;
	loff_t (*llseek) (struct file *, loff_t, int);
	ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
	ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
	int (*open) (struct inode *, struct file *);
	int (*flush) (struct file *, fl_owner_t id);
	int (*release) (struct inode *, struct file *);
	int (*fsync) (struct file *, loff_t, loff_t, int datasync);
	int (*fasync) (int, struct file *, int);
    ---------------------------------------------------------------------
    ---------------------------------------------------------------------
    ---------------------------------------------------------------------
};
````
+ __open__
```c
int pcd_open(struct inode *inode, struct file *filp);
````
+ __release__
```c
int pcd_release(struct inode *inode, struct file *filp);
````
| Parameter | Description |
|----------|-------|
| **inode** | Poiter of inode associated with filename. |
| **filp** | Poiter of file object. |

+ __read__
```c
    ssize_t pcd_read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos)
````
+ __write__
```c
    ssize_t pcd_write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos)
````
| Parameter | Description |
|----------|-------|
| **filp** | Poiter of file object. |
| **__user** | Option macro which alerts the programer that is a user level pointer so cannot be trusted for direct dereferencing. |
| **buff** | Poiter of of user buffer. |
| **count** | Read count given by user. |
| **f_pos** | Poiter of current file position from which the read has to begin. |

```c
	int copy_from_user(void *to, const void __user volatile *from, unsigned long n);
````
| Parameter | Description |
|----------|-------|
| **to** | Destination address, in user space. |
| **from** | Source address, in kernel space. |
| **n** | Number of byte to copy.|
```c
	int copy_to_user(void __user volatile *to, const void *from, unsigned long n)
````
| Parameter | Description |
|----------|-------|
| **to** | Destination address, in kernel space. |
| **from** | Source address, in user space. |
| **n** | Number of byte to copy.|

+ __llseek__
```c
    loff_t pcd_lseek(struct file *filp, loff_t off, int whence)
````
| Parameter | Description |
|----------|-------|
| **off** | Offset value. |
| **filp** | Poiter of file object. |
| **whence** | __SEEK_SET__, __SEEK_CUR__, __SEEK_END__. |

## Create device file
### class_create

Create a directory in sysfs :`/sys/class/<your_class_name>`
```c
struct class * class_create(struct module *owner, const char *name);
````
| Parameter | Description |
|----------|-------|
| **owner** | Pointer to the module that is to "own" this struct class. |
| **name** | String for the name of this class. |

### device_create
This function creates a subdirectory under `/sys/class/<your_class_name>` with your device name

This function also polulates sysfs entry with `dev` file which consists of the `major` and `minor` number, separated by a : character.

Populates the sysfs class you created in previous step with device numbers and device names
```c
struct device * device_create(struct class *class, struct device *parent, dev_t devt, void *drvdata, const char *fmt, ...);
````
| Parameter | Description |
|----------|-------|
| **class** | Pointer to the struct class that this device should be registered to. |
| **parent** | Pointer to the parent struct device of this new device. |
| **devt** | dev_t for the char device to be added. |
| **drvdata** | Data to be added to th device for callbacks. |
| **fmt** | String for the device's name. |