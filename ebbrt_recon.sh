#!/bin/bash

export RECON_DIR=~/EbbRT-fetalRecon
export RECONCMD=${RECON_DIR}/build/reconstruction

tranid=$1
threads=$(($2+1))
iterations=$3
backends=$4
mask=$5
frontEndCpus=$(($backends+1))

uploads="./uploads"
trandir="./transactions/${tranid}"

export EBBRT_NODE_ALLOCATOR_DEFAULT_CPUS=$threads
export EBBRT_NODE_ALLOCATOR_DEFAULT_RAM=4
export EBBRT_NODE_ALLOCATOR_DEFAULT_NUMANODES=1

function doRecon() {
    mkdir -p ${trandir}/slices ${trandir}/mask
    mv ${uploads}/* ${trandir}/slices

    if [ -z "${mask}" ]
    then
        MASK=""
    else
        mv ${uploads}/${mask} ${trandir}/mask/
        MASK="-m ${trandir}/mask/${mask}"
    fi

    cmd="${RECONCMD} -o ${trandir}/${tranid}.nii \
    -i ${trandir}/slices/*.nii \
    ${MASK} \
    --disableBiasCorrection --useAutoTemplate --useSINCPSF --resolution 2.0 \
    --debug 1 \
    --numThreads $threads --useCPU \
    --iterations $iterations --numNodes $backends \
    --numFrontEndCpus ${frontEndCpus}"

    echo ${cmd}
    $cmd
    
    cp ${trandir}/${tranid}.nii public/recon/
}

doRecon;
