#!/bin/bash


ergodic()
{
for file in ` ls $1 `
do
    if [ -d $1"/"$file ]
    then
        ergodic $1"/"$file
    else
        #echo $file
        if [ ${file}x = "Rootx" ]; then
            echo $1"/"$file
            echo :pserver:jxui@cvs:2401/var/cvs >> $1"/"$file
        fi
    fi
done
}

ergodic $1
