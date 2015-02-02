#!/bin/bash

git clone https://github.com/isagalaev/highlight.js.git tmp
(cd tmp; git checkout 8.4; npm install; node tools/build.js -t node;)

if [ -d ../Languages ]
then
	rm -rf ../Languages
fi

mkdir ../Languages

node build_syntax.js -i tmp/src/languages -o ../Languages
if [ $? != 0 ]
then
	echo "Error occured"
else
	echo "All themes has been refreshed successfully!"
fi
