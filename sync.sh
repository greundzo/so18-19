#!/bin/bash

if ["$1" -z]
	then git pull
else
[
	git add .
	git commit -m "$1"
	git push
]
fi
