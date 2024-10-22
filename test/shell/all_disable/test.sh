#!/usr/bin/env bash

output=$(./a.out)
if [ -z $output ]
then
	exit 0
else
	echo $output
	exit 1
fi

