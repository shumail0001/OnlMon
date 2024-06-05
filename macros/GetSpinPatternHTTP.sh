#!/bin/sh

packetID=$1

ssh -o ConnectTimeout=10 -o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no -l phnxrc -t operator1 "/home/repo/Debian/bin/getSpinPattern.py  /tmp/spinV.txt /tmp/spin14902.txt /tmp/spin14903.txt" &> /dev/null
scp "phnxrc@operator1:/tmp/spin1490?.txt" /tmp/  &> /dev/null
ssh -o ConnectTimeout=10 -o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no -l phnxrc -t operator1 "rm -i /tmp/spinV.txt /tmp/spin14902.txt /tmp/spin14903.txt" &> /dev/null
echo `cat /tmp/spin$packetID.txt 2> /dev/null`
rm -f /tmp/spin1490?.txt &> /dev/null

exit
