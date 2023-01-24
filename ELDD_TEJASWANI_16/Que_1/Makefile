obj-m:=que_1.o

ifdef ARCH
       
        KDIR = /lib/modules/5.10.65/build
else
        KDIR = /lib/modules/$(shell uname -r)/build
endif

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules
	gcc -o read read.c
	gcc -o write write.c

clean:  
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean

