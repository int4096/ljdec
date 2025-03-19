#!/bin/bash
sed -i "s/node/$1/g" cephs.sh
sed -i "s/sh1/$2/g" cephs.sh
sed -i "s/sh2/$3/g" cephs.sh
sed -i "s/sh3/$4/g" cephs.sh
