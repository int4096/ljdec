#!/bin/bash

#hexchars="0123456789ABCDEF"
#end=$( for i in {1..6} ; do echo -n ${hexchars:$(( $RANDOM % 16 )):1} ; done | sed -e 's/\(..\)/:\1/g' )
#mac=00:60:2F$end

#ip link set dev lo up
#ip link set dev bfg1 address $mac
#ip addr add 192.168.2.6/24 dev bfg1
#ip link set dev bfg1 up
#ip route add default via 192.168.2.1 dev bfg1
#ip addr

#ip link set dev bfg1 address $mac
#mkdir -p /guests/nginx && \
#chmod 0777 /guests/nginx && \
#debootstrap --include=sudo,nano,wget,gnupg,dirmngr,openssl,bridge-utils --arch amd64 bullseye /guests/$1 http://deb.debian.org/debian/

#apt-get update
#apt-get -y install openssh-server
#mkdir /run/sshd

apt-get update
apt-get -y install openssh-server
#wget -O- 'https://download.ceph.com/keys/release.asc' | sudo apt-key add -
#echo 'deb https://download.ceph.com/debian-quincy/ bullseye main' > /etc/apt/sources.list.d/ceph.list
#apt-get -y update
#apt -y install ceph ceph-base  ceph-common  ceph-fuse ceph-grafana-dashboards ceph-immutable-object-cache  ceph-mds  ceph-mgr \ 
#ceph-mgr-cephadm ceph-mgr-dashboard  ceph-mgr-k8sevents ceph-mgr-modules-core ceph-mgr-rook ceph-mon ceph-prometheus-alerts ceph-resource-agents \
#ceph-test cephadm cephfs-mirror  cephfs-shell cephfs-top libcephfs-dev libcephfs-java libcephfs-jni libcephfs2  librados-dev librados2  libradospp-dev \
#libradosstriper-dev libradosstriper1 librbd-dev librbd1 librgw-dev librgw2  libsqlite3-mod-ceph libsqlite3-mod-ceph-dev python3-ceph \
#python3-ceph-argparse python3-ceph-common python3-cephfs python3-rados python3-rbd python3-rgw rados-objclass-dev radosgw  rbd-fuse  rbd-mirror  rbd-nbd  
#cgroupfs-mount 
#/sbin/init
#cp /oldrootfs/etc/ssh/sshd_config /etc/ssh/
#cp -r /olfrootfs/root/.ssh/ /root/.ssh 
sh -c 'echo root:intgrea7 | chpasswd'
hostname l3
mount -t devpts devpts /dev/pts
/usr/sbin/sshd
#ip link set dev bfg1 address $mac
#/etc/init.d/ssh start 
#rm /etc/nginx/sites-enabled/*
#cp /jdec1/b.conf /etc/nginx/conf.d/

#nginx -g "daemon off;"

