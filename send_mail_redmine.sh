#!/bin/sh

# Nov 2018, Joe Xue

# Copyright (c) 2018 by lgxue@hotmail.com
# All rights reserved

from=$1
to=$2
subject="$3"
project="$4"
content="$5"

KEY="egLTAXMUpbZRMGtEn0Zj"
URL="http://redmine.wannlynx.com"
MYPATH=`dirname $0`
MAILHANDLER=${MYPATH}/rdm-mailhandler.rb

echo -e "\nSubject: $subject\nTo: $to\nFrom: $from\n\n${content}" | ${MAILHANDLER} --key ${KEY} --url ${URL} --project ${project}
