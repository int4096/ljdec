#!/bin/bash

hexchars="0123456789ABCDEF"
end=$( for i in {1..6} ; do echo -n ${hexchars:$(( $RANDOM % 16 )):1} ; done | sed -e 's/\(..\)/:\1/g' )
mac=00:60:2F$end

ip link set dev lo up
ip link set dev bfg1 address $mac
