obj-m += leds.o

KERN_DIR := /home/cheng/iTOP4412/iTop4412_Kernel_3.0

PWD ?= $(shell pwd)

all:
	make -C $(KERN_DIR) M=$(PWD) modules
	
clean:
	make -C $(KERN_DIR) M=$(PWD) modules clean
	rm -rf modules.order
