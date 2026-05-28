# Runs the QEMU machine

qemu-system-riscv64 \
    -machine virt -m 128M -smp 2 -nographic -serial mon:stdio \
    -bios /usr/share/qemu/opensbi-riscv64-generic-fw_dynamic.bin \
    -kernel build/bin/kernel.elf

# qemu-system-riscv64 \
#     -machine virt -m 128M -smp 2 -nographic \
#     -bios /usr/share/qemu/opensbi-riscv64-generic-fw_dynamic.bin \
#     -drive file=run/ovmf-code.bin,format=raw,if=pflash,readonly=off \
#     -drive file=run/ovmf-vars.bin,format=raw,if=pflash,readonly=off \
#     -drive file=run/disk.iso,format=raw,if=virtio

# qemu-system-riscv64 \
#     -machine virt \
#     -nographic \
#     -m 128M \
#     -kernel build/bin/kernel.elf \
#     -serial mon:stdio \
#     -gdb tcp::1234

#    -bios fw_dynamic.bin\
#    -d int,cpu_reset,guest_errors
#    -d int -D qemu.log \
#    -semihosting -semihosting-config enable=on,target=native

#    -smp 2 \
#    -drive file=disk.img,format=raw,if=virtio\
#   -d int,guest_errors \
