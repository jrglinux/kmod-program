COFNIG_MODULE_SIG = n

obj-m += poll_driver.o
KDIR = /lib/modules/$(shell uname -r)/build

all:
	gcc -o poll_user poll_user.c
	make -C $(KDIR)  M=$(shell pwd) modules
 
clean:
	make -C $(KDIR)  M=$(shell pwd) clean
	rm poll_user -f
