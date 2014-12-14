#!/bin/bash

host=${1:-localhost}
port=${2:-13050}

rm out.fifo in.fifo
mkfifo out.fifo in.fifo

nc $host $port < out.fifo | tee in.fifo \
                          | ./client | ./stitch.sh > out.fifo &

echo '<protocol><join gameType="swc_2015_hey_danke_fuer_den_fisch"/>' > out.fifo
