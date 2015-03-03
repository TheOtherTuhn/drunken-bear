#!/bin/bash
[[ -e "pipe" ]] && rm -f pipe
mkfifo pipe
nc ${1-127.0.0.1} ${2-13050} < pipe | ./run > pipe
