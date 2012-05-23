KDIR		:= /export/home/jxui/poslynx110/openwrt/build_dir/linux-lpc32xx/linux-2.6.34.7/
CROSS_COMPILE	= /export/home/jxui/poslynx110/openwrt/staging_dir/toolchain-arm_v5te_gcc-4.3.3+cs_uClibc-0.9.31_eabi/bin/arm-openwrt-linux-


obj-m += hello-1.o

all:
	make ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) -C $(KDIR) M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
