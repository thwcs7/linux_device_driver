echo "Building device tree"

cd /home/thwcs7/working_space/BBB/kernelbuildscripts/KERNEL
make -j16 ARCH=arm LOCALVERSION=-bone69 CROSS_COMPILE="/home/thwcs7/working_space/BBB/kernelbuildscripts/dl/gcc-8.5.0-nolibc/arm-linux-gnueabi/bin/arm-linux-gnueabi-" dtbs

echo "Finnish building"