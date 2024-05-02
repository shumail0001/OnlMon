#!/bin/bash
[[ -e htmlrunning ]] && exit 0
echo $$ > htmlrunning
source ./setup_all.sh
Xvfb :0 -nolisten tcp &
export DISPLAY=unix:0
perl makehtml.pl >& /sphenix/u/sphnxpro/makehmtl.log
kill $!
rm htmlrunning
