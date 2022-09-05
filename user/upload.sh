#!/bin/bash
# Serial upload script for Pocket265.
# Send binary user program to the Pocket265 via USB.
# $1 - tty
# $2 - binary file

set -e
ADDR="0800"

HEX=`xxd -ps $2`
HEX="@"$ADDR"#"$HEX"@"$ADDR"#"
HEX=`echo $HEX | sed -e 's/\(.\)/\1\n/g'`

stty -F $1 raw ospeed 200 ispeed 200

echo "Sending file $2 to $1"

for char in $HEX;
do
	echo -n $char > $1
	IFS= read -rd '' -n1 readback< $1
	echo -n $readback
done

echo -e "\nDone!"

