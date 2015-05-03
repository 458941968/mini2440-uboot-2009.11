cp u-boot.bin /mnt/hgfs/f
cp u-boot.map /mnt/hgfs/f
cp u-boot /mnt/hgfs/f/u-boot.axf
arm-linux-objdump -D u-boot>u-boot.s
cp u-boot.s /mnt/hgfs/f
cp System.map /mnt/hgfs/f
