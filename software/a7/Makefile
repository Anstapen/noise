TOOLCHAIN = /home/anton/Repositories/noise/buildroot/output/host/bin/arm-none-linux-gnueabihf-
DEBUG_FILE=main.debug


all: main.elf hello.elf

main.elf: main.c
	$(TOOLCHAIN)gcc -g -I../../linux/include -o main.elf main.c
#$(TOOLCHAIN)objcopy --only-keep-debug main.elf main.debug
#$(TOOLCHAIN)strip --strip-debug --strip-unneeded main.elf
#$(TOOLCHAIN)objcopy --add-gnu-debuglink=main.debug main.elf

hello.elf: hello.c
	$(TOOLCHAIN)gcc -o hello.elf -g -I../../linux/include hello.c

.PHONY: size-main size-hello clean all
size-main:
	$(TOOLCHAIN)size main

size-hello:
	$(TOOLCHAIN)size hello	

clean:
	rm main.elf hello.elf main.debug