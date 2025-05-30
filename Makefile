obj-m += hid-rakk-bulus.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	zstd -f hid-rakk-bulus.ko
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
install:
	cp $(shell pwd)/hid-rakk-bulus.ko.zst /lib/modules/$(shell uname -r)/kernel/drivers/hid/
	depmod -a
uninstall:
	rm $(shell pwd)/hid-rakk-bulus.ko.zst /lib/modules/$(shell uname -r)/kernel/drivers/hid/hid-rakk-bulus.ko.zst
	depmod -a