USBIP hardware emulation (Virtual USB device) 
=============

Tested with Linux Kernel 4.18<br>
Add suport to hso devices (c code).

Compile Linux Kernel with:<br>
CONFIG_USBIP_CORE=m<br>
CONFIG_USBIP_VHCI_HCD=m<br>

Compile and install usbip tool from Linux Kernel tree (tools/usb/usbip).

Compile hso emulation tool from this repo:<br>
$ cd c<br>
$ make<br>
$

From one terminal:<br>
$ ./hso<br> 
hso started....

From another terminal:<br>
$ sudo modprobe vhci-hcd<br>
$ sudo usbip attach -r 127.0.0.1 -b "1-1"<br>
$ ifconfig hso0<br>
hso0: flags=4240<POINTOPOINT,NOARP,MULTICAST>  mtu 1486<br>
        unspec 00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00  txqueuelen 10  (UNSPEC)<br>
        RX packets 0  bytes 0 (0.0 B)<br>
        RX errors 0  dropped 0  overruns 0  frame 0<br>
        TX packets 0  bytes 0 (0.0 B)<br>
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0<br>
$ rfkill list<br>
0: hso-0: Wireless WAN<br>
	Soft blocked: no<br>
	Hard blocked: no<br>
$ lsusb<br>
...<br>
Bus 003 Device 002: ID 0af0:6711 Option GlobeTrotter Express 7.2 v2<br>
...<br>
$

In order to release resources:<br>
$ usbip detach -p 00<br>
usbip: info: Port 0 is now detached!<br>
$ ifconfig hso0<br>
hso0: error fetching interface information: Device not found<br>
$ rfkill list<br>
$ lsusb<br>
...<br>
$

Device control with basic configuration for a correct initialization is completely emulated, data handling needs to be improved.
