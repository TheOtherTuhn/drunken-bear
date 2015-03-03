#!/bin/bash
[[ -e "pipe" ]] rm -f pipe
mkfifo pipe
nc $1 $2 < pipe | ./run > pipe
