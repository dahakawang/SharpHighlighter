#!/bin/bash

#git clone https://github.com/isagalaev/highlight.js.git tmp
(cd tmp; git checkout 8.4; npm install; node tools/build.js -t node;)

if [ ! -d ../Themes ]
then
	mkdir ../Themes
fi

node build_syntax.js -i tmp/src/languages -o ../Themes
if [ $? != 0 ]
then
	echo "Error occured"
else
	echo "All themes has been refreshed successfully!"
fi
