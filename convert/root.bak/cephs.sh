#!/bash
mount /dev/nikky/inna1 /mnt/inna1
mount /dev/nikky/inna2 /mnt/inna2
mount /dev/nikky/inna3 /mnt/inna3

./jdec /mnt/inna1 192.168.1.10 255.255.255.0 192.168.1.1 /sbin/init &
./jdec /mnt/inna2 192.168.1.11 255.255.255.0 192.168.1.1 /sbin/init &
./jdec /mnt/inna3 192.168.1.12 255.255.255.0 192.168.1.1 /sbin/init &

