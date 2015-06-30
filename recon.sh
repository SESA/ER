#!/bin/bash
#set -x
if [[ $# < 2 ]]; then
    echo "USAGE: $0 <dir> <responseurl>"
    exit -1;
fi

#dir=$1
#url=$2
#shift 2

#tran=$(basename $dir)
#STATUSFILE=$dir/status

export dir=$1
export url=$2
shift 2
export tran=$(basename $dir)

export MASKING_BASEDIR=${MASKING_BASEDIR:-$PWD/ext}
export MASKING_REPODIR=${MASKING_REPODIR:-$PWD/ext/example-motion-correction}
export RECON_REPODIR=${RECON_REPODIR:-$PWD/ext/fetalReconstruction}
#export STATUSFILE=$dir/status
export MASKCMD=${MASKING_REPODIR}/run.sh
export RECONCMD=${RECON_REPODIR}/source/bin/reconstruction_GPU2
#### help
export IRTK_DIR=$PWD/ext/IRTK
export PATH="$IRTK_DIR/bin:$PATH"
export PYTHONPATH=$IRTK_DIR/build/lib:$PYTHONPATH
# added by Anaconda 2.1.0 installer
export PATH="$PWD/ext/anaconda/bin:$PATH"

function setupDirectory()
{
    if [[ ! -d $dir ]]; then
	echo "ERROR: $dir does not exist";
	exit -1;
    fi
    
    if [[ ! -d $dir/data ]]; then
	mkdir $dir/data
    fi
    
    echo mv $@ $dir/data
    mv $@ $dir/data

    ln -s $MASKING_BASEDIR/model $dir/model

    #if ! mkfifo $STATUSFILE
    #then
    # 	echo "ERROR: cannot create status pipe $dir/status"
    # 	exit -1
    #fi    
}

# to generate list of mask files
# files=$(ls foo_* | sed 's/foo_//g' | sort -n | sed 's/^/foo_/g')
function doMask()
{
    echo dir = $dir
    echo "MASKING: START: $(date)"
    (
	cd $dir
	easy_install joblib
	$MASKCMD -12
    )
    echo "MASKING: END: $(date)"    
}

function doRecon()
{
    echo dir = $dir
    echo recon_cmd = $RECONCMD
    echo "RECON: START: $(date)"
    (
	cd $dir
	$RECONCMD -o $tran.nii -i output_segmentation/masked_* --disableBiasCorrection --useAutoTemplate --useSINCPSF --resolution 2.0 --debug 0 --numThreads 32 --useCPU
    )
    echo "RECON: END: $(date)"
    echo "RECON: output $url"
}

function finish()
{
    cp $dir/$tran.nii public/recon/$tran.nii
    #cp defaultrecon.nii public/recon/$tran.nii
    echo "slicedrop?$url$tran.nii"
}

echo "$0 $@ : STARTED: $(date)"

setupDirectory $@

#(
    echo "$(date) : $0 $@ : STARTED" 
    doMask
    doRecon
    finish
#) > $STATUSFILE
