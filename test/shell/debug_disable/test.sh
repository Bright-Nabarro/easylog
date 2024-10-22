#!/usr/bin/env bash

output=$(./a.out)

if [ -n "$output" ]; then
	exit 1
else
	exit 0
fi

