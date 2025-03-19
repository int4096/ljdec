#!/bin/bash

#if=ens3
HOST=$1
#/sbin/lvcreate -L +16G -n $1 int
#mkfs.ext4 -F /dev/int/$1
mkdir -p /hruck/$1
#mount /dev/int/$1 /media/$1
mkdir -p /hruck/$1 && \
chmod 0777 /hruck/$1 && \
debootstrap --include=sudo,nano,wget,gnupg,dirmngr,openssl,bridge-utils,openssh-server,nginx --arch amd64 bookworm /hruck/$1 http://deb.debian.org/debian
mkdir -p /hruck/$1/root/.ssh
cp -r /root/.ssh/ /hruck/$1/root/
echo $1 > /hruck/$1/etc/hostname
#cp mon1.sh /hruck/$1
cat > /hruck/$1/etc/hosts << "EOF"
127.0.0.1 localhost
192.168.1.10 sh1
192.168.1.11 sh2
192.168.1.12 sh3
192.168.1.13 sh4
192.168.1.14 sh5
EOF
#c\hroot /hruck/$1 bash /mon1.sh
#cp -r /home/int/root.bak /hruck/$1/root