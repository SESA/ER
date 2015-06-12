NPM=npm
GIT=git

.PHONY: clean all

all: ext/dropzone ext/slicedrop node_modules

ext/dropzone: .gitmodules
	${GIT} submodule init
	${GIT} submodule update

ext/slicedrop: .gitmodules
	${GIT} submodule init
	${GIT} submodule update

node_modules: package.json
	${NPM} install

run: all
	${NPM} start

clean:
	-${RM} -rf node_modules ext uploads
