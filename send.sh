#!/bin/bash

FILEINDEX=$(echo $1 | awk '{print substr($0,length($0),1)}')
NEWNAME=$(echo "response"$FILEINDEX)
touch $NEWNAME
echo $1
vim $1

PORT=80


nc $(cat $1 | grep "Host:" | awk '{printf "%s", $2}' | head -c -1) $PORT < $1 | tee $NEWNAME &
exit
