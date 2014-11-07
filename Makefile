ifneq ($(KERNELRELEASE),)
obj-m += chrono.o
chrono-objs :=  chronometre.o gpt.o display_fpga.o display.o display_gpio.o gpio.o

else
KDIR	:=~/workspace/buildroot/output/build/linux-3.7.9/
CPU  	:=arm
TOOLS 	:=~/workspace/buildroot/output/host/usr/bin/arm-linux-
PWD    	:=$(shell pwd)
MOD_PATH := /tftpboot/apf27-root

all:
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=$(CPU) CROSS_COMPILE=$(TOOLS)  modules

install:
	$(MAKE) -C $(KDIR) M=$(PWD) INSTALL_MOD_PATH=$(MOD_PATH) modules_install


clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
endif
