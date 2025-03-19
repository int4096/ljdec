#!/bin/bash
/sbin/init 3
ip addr add 10.0.2.4/24 dev bfg1
ip route add default via 10.0.2.1 dev bfg1
ip link set dev lo up
ip link set dev bfg1 address 00:60:2F:83:38:3D
ip link set dev bfg1 up
ip addr
ip link show bfg1
apt-get -y install php-7.4-fpm php7.4-curl php7.4-gd php7.4-mbstring php7.4-xml php7.4-xmlrpc php7.4-soap php7.4-intl php7.4-zip
/usr/sbin/php-fpm7.4 -F --pid /run/php/php-fpm.pid -y /etc/php/7.4/fpm/php-fpm.conf &
