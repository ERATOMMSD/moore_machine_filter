#!/bin/bash

readonly TEMPLATE_ID=lt-0ed1124f7e8e30dcf
readonly TEMPLATE_VERSION=4
readonly BUFF_SIZES="0 1 5 10 15 20 25 30 35 40 45 50 55 60 65 70"

for buf_size in $BUFF_SIZES;do
    tmpfile=$(mktemp /tmp/userdata-XXX.sh)
    m4 -DBUFFSIZE=$buf_size slave-user-data.sh.m4 > $tmpfile
    aws ec2 run-instances --launch-template "LaunchTemplateId=$TEMPLATE_ID,Version=$TEMPLATE_VERSION" --instance-type c4.large --userdata=file://$tmpfile
    rm -f $tmpfile
done
