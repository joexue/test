#!/bin/sh

recursive()
{
    curpath=$1
    #echo arg11=$curpath
    for files in `ls $curpath`
    do 
        if [ -d $1"/"$files ]; then
            curpath="${1}/${files}"
            echo $curpath
            recursive $curpath
        else
            echo $1"/"$files
        fi
    done
}

if [ -z $1 ];then
    path="./"
else
    path=$1
fi

recursive $path
