# If KERNELRELEASE is defined, we've been invoke from the 
# kernel build system and can use its language
ifneq ($(KERNELRELEASE),)
	scull-objs := main.o 
	obj-m := scull.o

# Otherwise we were called directly from the command
# line; invoke the kernel build system
else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build/
	PWD := $(shell pwd)
default:
	make -C $(KERNELDIR) M=$(PWD) modules
endif 

clean:
	make -C $(KERNELDIR) M=$(PWD) clean

