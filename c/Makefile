CC=gcc
CFLAGS= -Wall -DLINUX
CDC_ETHER_CFLAGS=${CFLAGS} -DMULTIPLE_CONFIGURATIONS -DNO_DEVICE_LIST -DNO_QUALIFIER -DBOS -DCDC_ETHER -DREDUCE_LOG -lpthread
PROGS= hid-mouse hid-keyboard cdc-acm hso cdc-ether
ifeq ($(DEBUG),YES)
CFLAGS+= -g -O
endif

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
		${CC} ${CFLAGS} usbip.c -c
		${CC} ${CFLAGS} usbip.o hso.c -o hso

cdc-ether:	usbip.c cdc-ether.c
		${CC} ${CDC_ETHER_CFLAGS} usbip.c -c
		${CC} ${CDC_ETHER_CFLAGS} usbip.o cdc-ether.c -o cdc-ether

clean:
		rm -f ${PROGS} core core.* *.o temp.* *.out typescript*
