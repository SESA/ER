NPM=npm
GIT=git

.PHONY: clean all irtk-build

all: ext/anaconda ext/fetalReconstruction/source/bin/reconstruction_GPU2 node_modules ext/IRTK/build/bin/reconstructionMasking

ext/Anaconda-2.1.0-Linux-x86_64.sh:
	cd ext && wget http://09c8d0b2229f813c1b93-c95ac804525aac4b6dba79b00b39d1d3.r79.cf1.rackcdn.com/Anaconda-2.1.0-Linux-x86_64.sh

irtk-build: ext/anaconda ext/IRTK/build/bin/reconstructionMasking

ext/IRTK/build/bin/reconstructionMasking:
	-mkdir ext/sesa-irtk/build
	cd ext/sesa-irtk/build && cmake -DPNG_LIBRARY:FILEPATH=$(CURDIR)/ext/anaconda/lib/libpng.so -DPNG_PNG_INCLUDE_DIR:DIRPATH=$(CURDIR)/ER/ext/anaconda/include -D BUILD_WITH_PNG=ON -D WRAP_CYTHON=ON -DBUILD_TESTS=OFF ..
	make -C ext/sesa-irtk/build  -j 8
	make -C ext/sesa-irtk/wrapping/cython/ext
	export PYTHONPATH=$(CURDIR)/ext/sesa-irtk/build/lib

ext/anaconda: ext/Anaconda-2.1.0-Linux-x86_64.sh
	cd ext && bash ./Anaconda-2.1.0-Linux-x86_64.sh -b -p $$PWD/anaconda
	export PATH=$(CURDIR)/ext/anaconda/bin:$$PATH && echo 'y' | conda install opencv && echo 'y' | conda install numpy=1.8
	sudo easy_install joblib


ext/fetalReconstruction/source/bin/reconstruction_GPU2:
	mkdir ext/fetalReconstruction/source/build
	cd ext/fetalReconstruction/source/build && cmake -DCMAKE_C_COMPILER=gcc-4.8 -DCMAKE_CXX_COMPILER=g++-4.8 -DBUILD_TESTS=OFF ..
	make -C ext/fetalReconstruction/source/build -j8 

node_modules: package.json
	${NPM} install

run: all
	-mkdir transactions
	${NPM} start

rm_sub_path:
	${GIT} rm --cached ext/dropzone
	${GIT} rm --cached ext/slicedrop

reset:
	${RM} -rf uploads/* transactions/* public/recon/* 

clean:
	-${RM} -rf node_modules uploads public/recon/* transactions/* 
