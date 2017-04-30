NPM=npm
GIT=git
CMAKE_PREFIX_PATH ?= "${HOME}/sysroots/release/hosted/"
EBBRT_SYSROOT ?= "${HOME}/sysroots/release/sysroot/"
CMAKE_TOOLCHAIN_FILE ?= "${HOME}/sysroots/release/sysroot/usr/misc/ebbrt.cmake"

.PHONY: clean all build

all: node_modules

node_modules: 
	${NPM} install

build:
	-cd ext/EbbRT-fetalRecon && mkdir -p build && cd build && cmake -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH} -DCMAKE_BUILD_TYPE=Release ../ && make -j
	-mkdir -p ext/EbbRT-fetalRecon/build/bm && cd ext/EbbRT-fetalRecon/build/bm && EBBRT_SYSROOT=${EBBRT_SYSROOT} cmake -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE} -DCMAKE_BUILD_TYPE=Release ../../ && make -j

run: all
	-mkdir -p public/recon
	-mkdir -p transactions
	-mkdir -p uploads
	-ln -sf `which nodejs` /usr/bin/node
	${NPM} start

reset:
	${RM} -rf uploads/* transactions/* public/recon/* 

clean:
	-${RM} -rf node_modules uploads public/recon/* transactions/* .*.BUILT
