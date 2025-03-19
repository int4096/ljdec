#!/bash
mount /dev/node/sh1 /mnt/sh1
mount /dev/node/sh2 /mnt/sh2
mount /dev/node/sh3 /mnt/sh3

./jdec /mnt/sh1 192.168.1.10 255.255.255.0 192.168.1.1 /sbin/init &
./jdec /mnt/sh2 192.168.1.11 255.255.255.0 192.168.1.1 /sbin/init &
./jdec /mnt/sh3 192.168.1.12 255.255.255.0 192.168.1.1 /sbin/init &

