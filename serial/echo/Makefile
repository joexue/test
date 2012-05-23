CROSS=/export/home/jxui/iPocketPlus/openwrt/staging_dir/toolchain-arm_v5te_gcc-4.3.3+cs_uClibc-0.9.31_eabi/bin/arm-openwrt-linux-

echoserv: echoserv.o helper.o
	${CROSS}gcc -o echoserv echoserv.o helper.o -Wall

serial: serial.o helper.o
	${CROSS}gcc -o serial serial.o helper.o -Wall

echoserv.o: echoserv.c helper.h
	${CROSS}gcc -o echoserv.o echoserv.c -c -ansi -pedantic -Wall

helper.o: helper.c helper.h
	${CROSS}gcc -o helper.o helper.c -c -ansi -pedantic -Wall

serial.o: serial.c helper.h
	${CROSS}gcc -o serial.o serial.c -c 


clean:
	rm *.o echoserv serial







