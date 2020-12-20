# Comment/uncomment the following line to disable/enable debugging
#DEBUG = y


# Add your debugging flag (or not) to CFLAGS
ifeq ($(DEBUG),y)
  DEBFLAGS = -O -g -DSBULL_DEBUG # "-O" is needed to expand inlines
else
  DEBFLAGS = -O2
endif

EXTRA_CFLAGS += $(DEBFLAGS)
EXTRA_CFLAGS += -I..

ifneq ($(KERNELRELEASE),)
# call from kernel build system

obj-m	:= sbull.o

else

KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD       := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

endif



clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions test1_1 test1_2 test2_1 test2_2 test3_1 test3_2

depend .depend dep:
	$(CC) $(EXTRA_CFLAGS) -M *.c > .depend


ifeq (.depend,$(wildcard .depend))
include .depend
endif

test1_1:
	gcc test1_1.c -o test1_1

test1_2:
	gcc test1_2.c -o test1_2

test2_1:
	gcc test2_1.c -o test2_1

test2_2:
	gcc test2_2.c -o test2_2

test3_1:
	gcc test3_1.c -o test3_1

test3_2:
	gcc test3_2.c -o test3_2

