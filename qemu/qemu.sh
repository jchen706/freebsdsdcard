#!/bin/sh

./qemu-system-aarch64 -m 4096M -cpu cortex-a53 -M virt  \
        -bios QEMU_EFI.fd -nographic \
        -drive if=none,file=FreeBSD-13.0-CURRENT-arm64-aarch64-RPI3-20200416-r359997.img,id=hd0 \
        -device virtio-blk-device,drive=hd0 \
        -device virtio-net-device,netdev=net0 \
        -netdev user,id=net0 \
        
