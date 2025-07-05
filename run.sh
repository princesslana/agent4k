#!/bin/sh
T=`mktemp`
tail -c +88 "$0"|xz -d|cc -o $T -O3 -xc -
(sleep 3;rm $T)&exec $T
