#!/usr/bin/env bash

ret=$(./a.out)

if [[ $ret -eq 2 ]]; then
	exit 1 		
else
	exit 0
fi

