#!/bin/bash
apt-get update
#ssh $4 "apt-get install ceph"
#ssh $5 "apt-get install ceph"
#ssh $6 "apt-get install ceph"
uuid=`uuidgen`
export uuid=$uuid
ssh $4 "chmod -R 777 /tmp"
ssh $5 "chmod -R 777 /tmp"
ssh $6 "chmod -R 777 /tmp"
sed -i "s/uuid/$uuid/g" ceph.conf
sed -i "s/sh1/$1/g" ceph.conf
sed -i "s/sh2/$2/g" ceph.conf
sed -i "s/sh3/$3/g" ceph.conf
sed -i "s/sh4/$4/g" ceph.conf
sed -i "s/sh5/$5/g" ceph.conf

sed -i "s/ip1/$6/g" ceph.conf
sed -i "s/ip2/$7/g" ceph.conf
sed -i "s/ip3/$8/g" ceph.conf
sed -i "s/ip4/$9/g" ceph.conf
sed -i "s/ip5/$10/g" ceph.conf


sed -i "s|subnet|$11|g" ceph.conf
cp ceph.conf /etc/ceph
sed -i "s/uuid/$uuid/g" start.sh
sed -i "s/sh1/$1/g" start.sh
sed -i "s/sh2/$2/g" start.sh
sed -i "s/sh3/$3/g" start.sh
sed -i "s/sh4/$4/g" start.sh
sed -i "s/sh5/$5/g" start.sh

sed -i "s/ip1/$6/g" start.sh
sed -i "s/ip2/$7/g" start.sh
sed -i "s/ip3/$8/g" start.sh
sed -i "s/ip4/$9/g" start.sh
sed -i "s/ip5/$10/g" start.sh

bash start.sh
bash scm $5
bash scm $6
bash smon $4 $1
bash smon $5 $2
bash smon $6 $3
ssh $4 "systemctl start ceph-mon\\@$1 && systemctl enable ceph-mon\\@$1"
ssh $5 "systemctl start ceph-mon\\@$2 && systemctl enable ceph-mon\\@$2"
ssh $6 "systemctl start ceph-mon\\@$3 && systemctl enable ceph-mon\\@$3"
mkdir -p /var/lib/ceph/mgr/ceph-$1
ceph -s
ceph auth get-or-create mgr.$1 mon 'allow profile mgr' osd 'allow *' mds 'allow *' > /var/lib/ceph/mgr/ceph-$1/keyring
ceph-mgr -i $1
#scp /var/lib/ceph/bootstrap-osd/ceph.keyring $5:/var/lib/ceph/bootstrap-osd/ceph.keyring
#scp /var/lib/ceph/bootstrap-osd/ceph.keyring $6:/var/lib/ceph/bootstrap-osd/ceph.keyring 
#ssh $4 "ceph-volume lvm create --data $8"
#ssh $5 "ceph-volume lvm create --data $9"
#ssh $6 "ceph-volume lvm create --data $10"