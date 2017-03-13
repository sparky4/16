#!/bin/bash
while [ -n "$1" ]
do
	7z a -v64k -r /var/www/"$1".zi "$1"
	echo 7z a -v64k -r /var/www/"$1".zi "$1"
	shift
done
