CC=/export/home/jxui/openwrt/openwrt/staging_dir/toolchain-arm_v5te_gcc-4.3.3+cs_uClibc-0.9.31_eabi/bin/arm-openwrt-linux-gcc
LD=/export/home/jxui/openwrt/openwrt/staging_dir/toolchain-arm_v5te_gcc-4.3.3+cs_uClibc-0.9.31_eabi/bin/arm-openwrt-linux-ld
LINUX_DIR=/export/home/jxui/openwrt/openwrt/build_dir/linux-poslynxmini/linux-2.6.34.7/

MODULE_NAME = regdump

obj-m		:= $(MODULE_NAME).o

all:
	$(MAKE) -C $(LINUX_DIR) ARCH="arm" M=$(PWD) modules

clean:
	rm -f *.o *.ko
