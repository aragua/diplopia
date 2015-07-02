#!/bin/sh

OUTPUT=/tmp/doublons/

function parse_directory
{
    printf '\033[1;32;40m'
    echo "Entering $1:"
    printf '\033[0m'
    IFS=$'\n'
    for file in $(ls -w 1 $1)
    do
	if [ -d $1/$file ]
	then
	    #echo "call parse_directory $file"
	    parse_directory $1/$file
	elif [ -L $1/$file ]
	then
	    printf '\033[0;33;40m'
	    echo "  $1/$file is a symlink"
	    printf '\033[0m'
	elif [ -f $1/$file ] && [ -r $1/$file ]
	then
	    #echo "compute md5 $file"
	    MD5=$(md5sum $1/$file | awk '{ print $1 }')
	    if ! [ -d $OUTPUT/$MD5 ]
	    then
		mkdir $OUTPUT/$MD5
	    else
		printf '\033[0;31;40m'
		echo "  $1/$file : $MD5"
		printf '\033[0m'
	    fi
	    ln -sf $1/$file $OUTPUT/$MD5/$file 
	else
	    echo $1/$file >> $OUTPUT/log.txt
	fi
    done
}

if [ $# -lt 1 ]
then
    echo "Error: need a folder to parse"
    exit 255
fi

if ! [ -d $1 ]
then
  echo "Error: $1 not a directory"
    exit 254
fi  

echo "Parsing $1 ..."

rm -rf $OUTPUT
mkdir $OUTPUT

parse_directory $(readlink -f $1)
