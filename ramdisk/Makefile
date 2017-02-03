#KERNELDIR=/usr/src/kernels/2.6.32-573.7.1.el6.x86_64/
KERNELDIR=/usr/src/kernels/2.6.32-642.6.2.el6.i686/
MAKE=make
PWD=$(shell pwd)


obj-m:= my_c_ramdisk.o

all:
	$(MAKE) -C $(KERNELDIR)  M=$(PWD)  modules
