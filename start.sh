#!/bin/bash
[[ -e "pipe" ]] && rm -f pipe
mkfifo pipe
nc ${2-127.0.0.1} ${4-13050} < pipe | ./run $6 > pipe
