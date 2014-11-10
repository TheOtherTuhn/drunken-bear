#!/bin/bash

host=${1:-localhost}
port=${2:-13050}

rm out.fifo in.fifo
mkfifo out.fifo in.fifo

nc $host $port < out.fifo | tee in.fifo \
                          | grep -Po '(roomId="\K[^"]*)|(Message" color="\K.)' > session.id &

echo '<protocol><join gameType="swc_2015_hey_danke_fuer_den_fisch"/>' > out.fifo

< in.fifo \
 | grep -Po '(currentPlayer="\K.")|(<field fish="\K\d)|(penguin owner="\K.)|(</room\K.)' | tr -d '\n' | tr '>' '\n' \
 | ./client \
 | ./stitch.sh \
 | tee out.fifo &
