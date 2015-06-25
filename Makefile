NPM=npm
GIT=git

.PHONY: clean all irtk-build


all: ext/anaconda ext/fetalReconstruction/source/bin/reconstruction_GPU2 node_modules 

ext/Anaconda-2.1.0-Linux-x86_64.sh:
	cd ext && wget http://09c8d0b2229f813c1b93-c95ac804525aac4b6dba79b00b39d1d3.r79.cf1.rackcdn.com/Anaconda-2.1.0-Linux-x86_64.sh

irtk-build: ext/anaconda ext/IRTK/build/bin/reconstructionMasking

ext/IRTK/build/bin/reconstructionMasking:
	-mkdir ext/IRTK/build
	cd ext/IRTK/build && cmake -DPNG_LIBRARY:FILEPATH=../anaconda/lib/libpng.so -DPNG_PNG_INCLUDE_DIR:DIRPATH=../anaconda/include -D BUILD_WITH_PNG=ON -D WRAP_CYTHON=ON ..
	make -C ext/IRTK/build  -j 8

ext/anaconda: ext/Anaconda-2.1.0-Linux-x86_64.sh
	cd ext && bash ./Anaconda-2.1.0-Linux-x86_64.sh -b -p $$PWD/anaconda

ext/fetalReconstruction/source/bin/reconstruction_GPU2:
	mkdir ext/fetalReconstruction/source/build
	cd ext/fetalReconstruction/source/build && cmake -DCMAKE_C_COMPILER=gcc-4.8 -DCMAKE_CXX_COMPILER=g++-4.8 -DBUILD_TESTS=OFF ..
	make -C ext/fetalReconstruction/source/build -j8 

node_modules: package.json
	${NPM} install

run: all
	${NPM} start

rm_sub_path:
	${GIT} rm --cached ext/dropzone
	${GIT} rm --cached ext/slicedrop

reset:
	${RM} -rf uploads/* transactions/* public/recon/* 

clean:
	-${RM} -rf node_modules uploads public/recon/* transactions/*
