#!/bin/bash
#set -x
echo "$0 $@ : STARTED: $(date)"

if [[ $# < 1 ]]; then
    echo "USGE: $0 <dir>"
    exit -1;
fi

dir=$1
STATUSFILE=$dir/status

if [[ ! -d $dir ]]; then
    echo "ERROR: $dir does not exist";
    exit -1;
fi

if ! mkfifo $STATUSFILE
then
    echo "ERROR: cannot create status pipe $dir/status"
    exit -1
fi

(
echo "$(date) : $0 $@ : STARTED" 
echo "MASKING: START: $(date)"  
sleep 2
echo "MASKING: END: $(date)"
echo "RECON: START: $(date)" 
sleep 2
echo "RECON: END: $(date)"
echo "RECON: output url"
echo ${dir}recon.nii
) > $STATUSFILE
