#!/bin/bash

# change user
su mwaga
cd

# create experiment environment
wget URI
tar xvf moore_machine_filter.tar.xz

# execute the experiment
cd moore_machine_filter
mkdir build
cd experiments/ && ./run_gtime_maunaloa_slave.sh BUFFSIZE

# copy logs
aws s3 cp log/ s3://mmm-mwaga/logs

# stop/terminate instance
aws ec2 ifdef(`TERMINATE',`terminiate', `stop')-instances $(curl http://169.254.169.254/latest/meta-data/ami-id)
