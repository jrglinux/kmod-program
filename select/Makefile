COFNIG_MODULE_SIG = n

obj-m += select_driver.o
KDIR = /lib/modules/$(shell uname -r)/build

all:
	gcc -o select_user select_user.c
	make -C $(KDIR)  M=$(shell pwd) modules
 
clean:
	make -C $(KDIR)  M=$(shell pwd) clean
	rm select_user -f
