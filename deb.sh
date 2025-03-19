#!/bin/bash

mkdir -p /home/mnt2/$1 && \
chmod 0777 /home/mnt2/$1 && \
debootstrap --include=nano,wget,gnupg,dirmngr,openssl,bridge-utils,openssh-server --arch amd64 bullseye /home/mnt2/$1 http://deb.debian.org/debian/
