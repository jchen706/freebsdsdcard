We utilized FreeBSD 13.0 CURRENT image.

The image can be downloaded from here : https://download.freebsd.org/ftp/snapshots/arm64/aarch64/ISO-IMAGES/13.0/. 

Expect the source file to be change every day, so I forked it on the 4/17 for the 4/16 released image. 
The forked FreeBSD link is here: https://github.com/jchen706/freebsd

In the qemu folder, there is the qemu.sh. However, the qemu-system-aarch64 was too large to upload. Therefore, you can download qemu on command line on linux with apt-get install qemu . You can use the qemu-img command to run the image.


If you are making a new image, you can add cores by typing "-smp 4" to add four cores to the qemu image when using qemu command.

Currently, what we have is a FreeBSD kernel module which is the skeleton.c. The Rust module is /freebsdsdcard/src/lib.rs . The Global Allocator is in allocator.rs which is /freebsdsdcard/src/allocator.rs.  The allocator functions import kernal alloc and dealloc  functions from /freebsdsdcard/src/kernel_malloc.rs.   The whatif.h is the header file importing the Rust function to the C kernel module. In the skeleton.c, #include “whatif.h” is one of the headers. To fully link the Rust functions, in the FreeBSD command line, “env RUSTC_BOOTSTRAP=1 cargo build --release” was required for the Makefile in /freebsdsdcard/src/Makefile to have ”LDFLAGS= -L. ../target/release/libfreebsdsdcard.a”.  First we have to “make” , then to generate the object files of the kernel module. To load the kernel module into the kernel space, we can do “make load” which will print out the value of the Box from the allocator. The command “kldstat” will show if the kernel module is loaded or unloaded into the kernel. To unload the kernel module “make unload”., and “make clean” will clear all dynamically generated 









