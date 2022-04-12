CC:=gcc
CFLAGS:= -Wall -DLINUX
ifeq ($(DEBUG),YES)
CFLAGS+= -g -O
endif
HSO_CFLAGS:=${CFLAGS} -DCONFIGURABLE_SERVER_USBIP_TCP_PORT -DCONFIGURABLE_USB_BUS_PORT -DVERBOSE_LEVEL
CDC_ETHER_CFLAGS:=${CFLAGS} -DMULTIPLE_CONFIGURATIONS -DNO_DEVICE_LIST -DNO_QUALIFIER -DBOS -DCONFIGURABLE_SERVER_USBIP_TCP_PORT -DCONFIGURABLE_USB_BUS_PORT -DSYNC_DATA_STOP -DCDC_ETHER -DVERBOSE_LEVEL -lpthread
BT_CFLAGS:=${CFLAGS} -DMULTIPLE_CONFIGURATIONS -DNO_DEVICE_LIST -DNO_QUALIFIER -DCONFIGURABLE_SERVER_USBIP_TCP_PORT -DCONFIGURABLE_USB_BUS_PORT -DVERBOSE_LEVEL -DUNLINK_ANSWER -DKEEP_START_FRAME
PROGS:= hid-mouse hid-keyboard cdc-acm hso cdc-ether bt

SPEC_FILE=usbip-emulator.spec
TAR_PACKAGE_NAME:=$(shell grep "^Name:" ${SPEC_FILE} | cut -d ":" -f 2 | tr -d ' ')
TAR_PACKAGE_VERSION:=$(shell grep "^Version:" ${SPEC_FILE} | cut -d ":" -f 2 | tr -d ' ')

all:	${PROGS}

hid-mouse:	usbip.c hid-mouse.c 
		${CC} ${CFLAGS} usbip.c -c 
		${CC} ${CFLAGS} usbip.o hid-mouse.c -o hid-mouse 

hid-keyboard:	usbip.c hid-keyboard.c 
		${CC} ${CFLAGS} usbip.c -c 
		${CC} ${CFLAGS} usbip.o hid-keyboard.c -o hid-keyboard
	
cdc-acm:	usbip.c cdc-acm.c 
		${CC} ${CFLAGS} usbip.c -c 
		${CC} ${CFLAGS} usbip.o cdc-acm.c -o cdc-acm

hso:	usbip.c hso.c
		${CC} ${HSO_CFLAGS} usbip.c -c
		${CC} ${HSO_CFLAGS} usbip.o hso.c -o hso

cdc-ether:	usbip.c cdc-ether.c
		${CC} ${CDC_ETHER_CFLAGS} usbip.c -c
		${CC} ${CDC_ETHER_CFLAGS} usbip.o cdc-ether.c -o cdc-ether

bt:	usbip.c bt.c
		${CC} ${BT_CFLAGS} usbip.c -c
		${CC} ${BT_CFLAGS} usbip.o bt.c -o bt

rpm:
		mkdir -p ${TAR_PACKAGE_NAME}-${TAR_PACKAGE_VERSION}
		cp *.c *.h Makefile ${TAR_PACKAGE_NAME}-${TAR_PACKAGE_VERSION}
		tar -zcv ${TAR_PACKAGE_NAME}-${TAR_PACKAGE_VERSION} -f ${TAR_PACKAGE_NAME}-${TAR_PACKAGE_VERSION}.src.tar.gz
		rm -rf ${TAR_PACKAGE_NAME}-${TAR_PACKAGE_VERSION}
		mkdir -p ~/rpmbuild/SOURCES
		mv ${TAR_PACKAGE_NAME}-${TAR_PACKAGE_VERSION}.src.tar.gz ~/rpmbuild/SOURCES
		rpmbuild -bb ${SPEC_FILE}

clean:
		rm -f ~/rpmbuild/SOURCES/${TAR_PACKAGE_NAME}-${TAR_PACKAGE_VERSION}.src.tar.gz
		rm -rf /home/jtornosm/rpmbuild/BUILD/${TAR_PACKAGE_NAME}-${TAR_PACKAGE_VERSION}
		rm -rf ${TAR_PACKAGE_NAME}-${TAR_PACKAGE_VERSION}
		rm -f ${PROGS} core core.* *.o temp.* *.out typescript*
