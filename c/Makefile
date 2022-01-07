CC=gcc
CFLAGS= -Wall -DLINUX 
PROGS= hid-mouse hid-keyboard cdc-acm hso

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

clean:
		rm -f ${PROGS} core core.* *.o temp.* *.out typescript*
