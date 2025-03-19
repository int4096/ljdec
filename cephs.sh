#!/bash
./jdec /guests/sh1 192.168.1.10 255.255.255.0 192.168.1.1 /sbin/init &
sleep 5
./jdec /guests/sh2 192.168.1.11 255.255.255.0 192.168.1.1 /sbin/init &
sleep 5
./jdec /guests/sh3 192.168.1.12 255.255.255.0 192.168.1.1 /sbin/init &

