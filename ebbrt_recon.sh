#!/bin/bash
export RECON_DIR=~/EbbRT-fetalRecon
export RECONCMD=${RECON_DIR}/build/reconstruction


tranid=$1
threads=$(($2+1))
iterations=$3
backends=$4
mask=$5

uploads="./uploads"
trandir="./transactions/${tranid}"

function doRecon() {
    echo ${trandir}
    mkdir -p ${trandir}/slices ${trandir}/mask
    mv ${uploads}/* ${trandir}/slices

    if [ -z "${mask}" ]
    then
        MASK=""
    else
        mv ${uploads}/${mask} ${trandir}/mask/
        MASK="-m ${trandir}/mask/${mask}"
    fi

    ${RECONCMD} -o ${trandir}/${tranid}.nii \
    -i ${trandir}/slices/*.nii \
    ${MASK} \
    --disableBiasCorrection --useAutoTemplate --useSINCPSF --resolution 2.0 \
    --debug 1 \
    --numThreads $threads --useCPU \
    --iterations $iterations --numNodes $backends \
    --numFrontEndCpus $(($backends+1))
    
    cp ${trandir}/${tranid}.nii public/recon/
}

doRecon;
