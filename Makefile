NPM=npm
GIT=git

.PHONY: clean all

init: 
	${GIT} submodule add git@github.com:slicedrop/slicedrop.github.com.git ext/slicedrop
	${GIT} submodule add git@github.com:enyo/dropzone.git ext/dropzone

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

rm_sub_path:
	${GIT} rm --cached ext/dropzone
	${GIT} rm --cached ext/slicedrop

clean:
	-${RM} -rf node_modules uploads
