#!/bin/bash

exec 3<"$1"

while read -r -u3 line || [[ -n "$line" ]]; do
	echo "$line"
	read -s -n1
done
