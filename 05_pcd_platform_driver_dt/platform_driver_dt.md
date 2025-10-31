# Device tree

### Property: compatible

````c
compatible = <string-list>
````

e.g:

compatible = "string1", "string2", "string3";

+ Compatible property of a device node is used by the Linux kernel to match and load an appropriate driver to handle that device node (driver selection)
+ The string list provided by the compatible property is matched against the string list supported by the driver. If the kernel finds any match, the driver will be loaded, and probe function is invoked

Direction `/sys/devices/platform/name_of_your_device` represent a device. You see some attributes of this device.
````
root@arm:/sys/devices/platform/pcdev-1# ls
driver_override  modalias  of_node  power  subsystem  uevent
````
Direction `/sys/devices/platform/name_of_your_device/of_node` represent a node of device (device node). All the properties is here
````
root@arm:/sys/devices/platform/pcdev-1/of_node# ls
compatible  name  org,device-serial-num  org,perm  org,size
````