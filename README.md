USBIP hardware emulation (Virtual USB device) 
=============

Tested with Linux Kernel 4.18
Add suport to hso devices (c code).

Compile Linux Kernel with:
CONFIG_USBIP_CORE=m
CONFIG_USBIP_VHCI_HCD=m

Compile and install usbip tool from Linux Kernel tree (tools/usb/usbip).

Compile hso emulation tool from this repo:
$ cd c
$ make
$

From one terminal:
$ ./hso
hso started....

From another terminal:
$ sudo modprobe vhci-hcd
$ sudo usbip attach -r 127.0.0.1 -b "1-1"
$ ifconfig hso0
hso0: flags=4240<POINTOPOINT,NOARP,MULTICAST>  mtu 1486
        unspec 00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00  txqueuelen 10  (UNSPEC)
        RX packets 0  bytes 0 (0.0 B)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 0  bytes 0 (0.0 B)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0
$ rfkill list
0: hso-0: Wireless WAN
	Soft blocked: no
	Hard blocked: no
$ lsusb
...
Bus 003 Device 002: ID 0af0:6711 Option GlobeTrotter Express 7.2 v2
...
$

In order to release resources:
$ usbip detach -p 00
usbip: info: Port 0 is now detached!
$ ifconfig hso0
hso0: error fetching interface information: Device not found
$ rfkill list
$ lsusb
...
$


Device control with basic configuration for a correct initialization is completely emulated, data handling needs to be improved.
