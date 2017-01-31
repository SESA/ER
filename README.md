# ER - Elastic Reconstruction

## Requirements

- build-essential
- cmake
- nodejs
- npm
- python-setuptools
- libgsl0-dev
- libboost

## Clone repo and install submodules
    $> git clone https://github.com/SESA/ER.git
    $> cd ER
    $> git submodule update --init --recursive
    
## How to build
    $> cd fetalReconstruction/source/build
    $> cmake .. 
    $> make -j
    
## How to run
    $> nodejs ./bin/www
