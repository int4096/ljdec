scp -r /etc/ceph $1:/etc
scp -r /tmp/* $1:/tmp
ssh $1 chown -R ceph:ceph /tmp/
