#!/bin/bash
while [ -n "$1" ]
do
	7z a -v64k /var/www/"$1".zip "$1"
	shift
done
