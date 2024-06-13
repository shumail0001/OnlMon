#!/bin/sh

#packetID="14902"
#type="physics"
#run=$(printf "%08d" 44238)

packetID=$1
type=$2
run=$(printf "%08d" $3)
echo `ddump -p $packetID -t S -g -d -f /sphenix/lustre01/sphnxpro/physics/GL1/$type/GL1_${type}_gl1daq-$run-0000.evt| awk 'NR>1 {print $0} '| cut -d"|" -f2  2> /dev/null`
#echo `ddump -p $packetID -t S -g -d -f /sphenix/lustre01/sphnxpro/commissioning/GL1/$type/GL1_${type}_gl1daq-$run-0000.evt| awk 'NR>1 {print $0} '| cut -d"|" -f2  2> /dev/null`

exit
