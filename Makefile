NPM=npm
GIT=git

.PHONY: clean all

all: ext/anaconda .masking.BUILT .reconstruction.BUILT node_modules

ext/Anaconda-2.1.0-Linux-x86_64.sh:
	cd ext && wget http://09c8d0b2229f813c1b93-c95ac804525aac4b6dba79b00b39d1d3.r79.cf1.rackcdn.com/Anaconda-2.1.0-Linux-x86_64.sh

.masking.BUILT: 
	-mkdir ext/IRTK/build
	cd ext/IRTK/build && cmake -DPNG_LIBRARY:FILEPATH=$(CURDIR)/ext/anaconda/lib/libpng.so -DPNG_PNG_INCLUDE_DIR:DIRPATH=$(CURDIR)/ext/anaconda/include -D BUILD_WITH_PNG=ON -D WRAP_CYTHON=ON -D BUILD_TESTS=OFF ..
	make -C ext/IRTK/build -j 8
	make -C ext/IRTK/wrapping/cython/ext
	touch .masking.BUILT

ext/anaconda: ext/Anaconda-2.1.0-Linux-x86_64.sh
	cd ext && bash ./Anaconda-2.1.0-Linux-x86_64.sh -b -p $$PWD/anaconda
	export PATH=$(CURDIR)/ext/anaconda/bin:$$PATH && echo 'y' | conda install opencv && echo 'y' | conda install numpy=1.8
	sudo easy_install joblib

.reconstruction.BUILT:
	-mkdir ext/fetalReconstruction/source/build
	cd ext/fetalReconstruction/source/build && cmake -DCMAKE_C_COMPILER=gcc-4.8 -DCMAKE_CXX_COMPILER=g++-4.8 -DBUILD_TESTS=OFF ..
	make -C ext/fetalReconstruction/source/build -j8 
	touch .reconstruction.BUILT

node_modules: 
	${NPM} install

run: all
	-mkdir public/recon
	-mkdir transactions
	${NPM} start

reset:
	${RM} -rf uploads/* transactions/* public/recon/* 

clean:
	-${RM} -rf node_modules uploads public/recon/* transactions/* .*.BUILT
