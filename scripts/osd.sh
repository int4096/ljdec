scp /var/lib/ceph/bootstrap-osd/ceph.keyring $2:/var/lib/ceph/bootstrap-osd/ceph.keyring
scp /var/lib/ceph/bootstrap-osd/ceph.keyring $3:/var/lib/ceph/bootstrap-osd/ceph.keyring
scp /var/lib/ceph/bootstrap-osd/ceph.keyring $4:/var/lib/ceph/bootstrap-osd/ceph.keyring
scp /var/lib/ceph/bootstrap-osd/ceph.keyring $5:/var/lib/ceph/bootstrap-osd/ceph.keyring
ssh $1 "ceph-volume lvm create --data $4"
ssh $2 "ceph-volume lvm create --data $5"
ssh $3 "ceph-volume lvm create --data $6"
ssh $3 "ceph-volume lvm create --data $7"
ssh $3 "ceph-volume lvm create --data $8"
