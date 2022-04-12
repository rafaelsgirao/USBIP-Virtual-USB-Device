KERNEL_REPO?=git@gitlab.com:redhat/rhel/src/kernel/rhel-8.git
KERNEL_REPO_BRANCH?=main
KERNEL_REPO_DIR:=$(shell basename $(KERNEL_REPO) | cut -d '.' -f 1)
USBIP_PATH:=tools/usb/usbip
USBIP_DIR?=$(shell basename $(USBIP_PATH))
USBIP_SRC_TMP?=/tmp/usbip

SPEC_FILE=usbip.spec
TAR_PACKAGE_NAME:=$(shell grep "^Name:" ${SPEC_FILE} | cut -d ":" -f 2 | tr -d ' ')
TAR_PACKAGE_VERSION:=$(shell grep "^Version:" ${SPEC_FILE} | cut -d ":" -f 2 | tr -d ' ')

SHELL := /bin/bash

all: download compile

download:
	if [ ! -d ${USBIP_DIR} ];then \
		mkdir -p ${USBIP_SRC_TMP}; \
		pushd ${USBIP_SRC_TMP}; \
		git clone --filter=blob:none --no-checkout ${KERNEL_REPO}; \
        	cd ${KERNEL_REPO_DIR}; \
		git checkout ${KERNEL_REPO_BRANCH}; \
		git sparse-checkout set ${USBIP_PATH}; \
		popd; \
		cp -r ${USBIP_SRC_TMP}/${KERNEL_REPO_DIR}/${USBIP_PATH} .; \
	fi;

compile:
	cd ${USBIP_DIR}; \
	./autogen.sh; \
	./configure --prefix=/usr/; \
	make

install:
	cd ${USBIP_DIR}; \
	make install

src: download
	mkdir -p ${TAR_PACKAGE_NAME}-${TAR_PACKAGE_VERSION}
	cp -r ${USBIP_DIR} ${TAR_PACKAGE_NAME}-${TAR_PACKAGE_VERSION}
	cp Makefile ${TAR_PACKAGE_NAME}-${TAR_PACKAGE_VERSION}
	cp ${SPEC_FILE} ${TAR_PACKAGE_NAME}-${TAR_PACKAGE_VERSION}
	tar -zcv ${TAR_PACKAGE_NAME}-${TAR_PACKAGE_VERSION} -f ${TAR_PACKAGE_NAME}-${TAR_PACKAGE_VERSION}.src.tar.gz
	rm -rf ${TAR_PACKAGE_NAME}-${TAR_PACKAGE_VERSION}
	mkdir -p ~/rpmbuild/SOURCES
	mv ${TAR_PACKAGE_NAME}-${TAR_PACKAGE_VERSION}.src.tar.gz ~/rpmbuild/SOURCES             

rpm: src
	rpmbuild -bb ${SPEC_FILE}

srpm: src
	rpmbuild -bs ${SPEC_FILE}

clean:
	if [ -d ${USBIP_DIR} ];then \
		rm -f ~/rpmbuild/SOURCES/${TAR_PACKAGE_NAME}-${TAR_PACKAGE_VERSION}.src.tar.gz; \
		rm -rf /home/jtornosm/rpmbuild/BUILD/${TAR_PACKAGE_NAME}-${TAR_PACKAGE_VERSION}; \
		cd ${USBIP_DIR}; \
		./cleanup.sh; \
	fi;

mrproper:
	rm -rf ${USBIP_SRC_TMP}
	rm -rf ${USBIP_DIR}
