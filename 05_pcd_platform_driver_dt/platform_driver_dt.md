# Device tree

### Property: compatible

````c
compatible = <string-list>
````

e.g:

compatible = "string1", "string2", "string3";

+ Compatible property of a device node is used by the Linux kernel to match and load an appropriate driver to handle that device node (driver selection)
+ The string list provided by the compatible property is matched against the string list supported by the driver. If the kernel finds any match, the driver will be loaded, and probe function is invoked

