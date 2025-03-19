#!/bin/bash
hexchars="0123456789ABCDEF"
end=$( for i in {1..6} ; do echo -n ${hexchars:$(( $RANDOM % 16 )):1} ; done | sed -e 's/\(..\)/:\1/g' )
mac=00:60:2F$end
ip addr add 10.0.1.2/24 dev bfg1
ip link set dev lo up
ip link set dev bfg1 address $mac
ip link set dev bfg1 up
ip route add default via 10.0.0.1 dev bfg1
ip addr

apt-get clean
apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 3A79BD29
cat > /etc/apt/sources.list.d/mysql.list << "EOF"
deb http://repo.mysql.com/apt/debian/ bullseye mysql-8.0
deb http://repo.mysql.com/apt/debian/ bullseye mysql-tools
EOF
groupadd mysql
useradd mysql -b /var/lib/mysql -s /sbin/nologin -g mysql
apt-get update
apt-get install -y mysql-community-client mysql-community-server-core
chown -R mysql:mysql /var/lib/mysql /var/run/mysqld 
chmod 1777 /var/run/mysqld /var/lib/mysql
mkdir -p /var/lib/mysql
mkdir /var/run/mysqld
mkdir /var/lib/mysql-files /var/run/mysqld
chown mysql:mysql /var/lib/mysql /var/lib/mysql-files /var/run/mysqld
mysqld --initialize --user=mysql
echo "ALTER USER 'root'@'localhost' IDENTIFIED BY 'seledka';" > /init
sudo -u mysql mysqld --bind-address=10.0.1.2 init-file=/init &
rm -v /init

