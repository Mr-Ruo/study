KERN_DIR = /opt/workspace/linux-3.2.0-psp04.06.00.10/

all:
	make -C $(KERN_DIR) M=`pwd` modules 
clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm -rf modules.order
obj-m	+= virtualcdev.o
