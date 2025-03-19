#!/bin/bash
ip addr add 10.0.2.3/24 dev bfg1
ip route add default via 10.0.2.1 dev bfg1
ip link set dev lo up
ip link set dev bfg1 address 00:60:2F:83:38:3D
ip link set dev bfg1 up
ip addr
ip link show bfg1

apt-get update
apt-get -y install nginx
rm -R /etc/nginx/sites-enabled/*
cp b.conf /etc/nginx/conf.d/

nginx -g "daemon off;" &