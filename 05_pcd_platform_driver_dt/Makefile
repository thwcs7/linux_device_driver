obj-m += pcd_platform_driver_dt.o

ARCH=arm
CROSS=/home/thwcs7/working_space/BBB/kernelbuildscripts/dl/gcc-8.5.0-nolibc/arm-linux-gnueabi/bin/arm-linux-gnueabi-
KER_DIR=/home/thwcs7/working_space/BBB/kernelbuildscripts/KERNEL
HOST_KER_DIR = /lib/modules/$(shell uname -r)/build 
GCC_APP_DIR = /home/thwcs7/working_space/BBB/gcc-linaro-6.5.0-2018.12-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc

all:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS) -C $(KER_DIR) M=$(PWD) modules

clean:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS) -C $(KER_DIR) M=$(PWD) clean
	rm -rf app

help:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS) -C $(KER_DIR) M=$(PWD) help

host:
	make -C $(HOST_KER_DIR) M=$(PWD) modules

build_app:
	$(GCC_APP_DIR) -o app app.c

copy:
	scp pcd_platform_driver_dt.ko debian@10.42.0.175:/home/debian/

copy-dtb:
	scp /home/thwcs7/working_space/BBB/kernelbuildscripts/KERNEL/arch/arm/boot/dts/am335x-boneblack.dtb debian@10.42.0.175:/home/debian/
	
