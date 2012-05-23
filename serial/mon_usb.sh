#!/bin/sh

# 05,12 2011    Created by Joe Xue at Precidia

USB_CH=/dev/ttyUSB3
exec 3<> $USB_CH

# for close exec 3>&-

my_read()
{
    local c
    local string
    while [ 1 ]; do
        read -t 1  c <&3
        if [ $? -eq 0 ]; then
            string=`echo ${string}${c}`
            #echo $c
            #echo $string
        else
            break
        fi
    done

    eval $1=$string
    return 0    
}



echo  "at" >&3
my_read 
echo "at+cnmi=2,1" >&3
my_read

echo  "at" >&3
my_read

echo at+cmgr=0 >&3
my_read content
echo $content

