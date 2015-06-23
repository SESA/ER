#!/bin/bash
#set -x
echo "$0 $@ : STARTED: $(date)"

if [[ $# < 2 ]]; then
    echo "USAGE: $0 <dir> <responseurl>"
    exit -1;
fi

dir=$1
url=$2
shift 2

tran=$(basename $dir)
STATUSFILE=$dir/status

if [[ ! -d $dir ]]; then
    echo "ERROR: $dir does not exist";
    exit -1;
fi

echo mv $@ $dir
mv $@ $dir

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

cp data/defaultrecon.nii public/recon/$tran.nii
echo "http://localhost:3000/sliceDrop?$url$tran.nii"
) > $STATUSFILE
