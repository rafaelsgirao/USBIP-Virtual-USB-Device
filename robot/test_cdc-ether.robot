*** Settings ***
Documentation	cdc-ether test script.
...		It is necessary to configure peer

Library		SSHLibrary
Suite Setup	Open Connection And Log In	
Suite Teardown	Close All Connections
Resource	test_common.robot

*** Variables ***
${HOST}		192.168.122.252
${USERNAME}	robot_test
${PASSWORD}	robot_test
${ALIAS}	cdc-ether_test
${JOURNALCTL_LOG}	/tmp/cdc-ether_journalctl.log
${EMULATOR}	cdc-ether
${EMULATOR_LOG}	/tmp/cdc-ether.log
${TCP_PORT}	3242
${BUS_PORT}	1-1
${INTERFACE_TO_BIND}   enp1s0 
${DEVICE}	eth0
${IP_ADDRESS}	10.0.0.1
${PEER_IP_ADDRESS}	10.0.0.2
${IP_MASK}	24
${PING_NUMBER}	5

*** Test Cases ***
Init cdc-ether
	[Documentation]	Init emulated cdc-ether device and check 
	Start Emulator	-i ${INTERFACE_TO_BIND}	1
	Load usbip Modules
	Attach usbip
	Sleep	1
	Check Device	0

Check Network
	[Documentation]	Check network for emulated cdc-ether device
	Configure Network
	Sleep	1
	Check Network	0

Release cdc-ether
	[Documentation]	Release emulated cdc-ether device and check
	Detach usbip
	Check Device	1
	Unload usbip Modules
	Kill Emulator	root=1
	Check Network	1

Check logs
	Check Emulator Log
	Check journalctl Log

*** Keywords ***
Check Device
	[Documentation]	Check device
	[Arguments]	${result}
	${output}	${stderr}	${rc}=	Execute Command	ip addr show dev ${DEVICE}	sudo=True	sudo_password=${PASSWORD}	return_stderr=True	return_rc=True
	Log	${output}
	Run Keyword If	${rc}	Log	${stderr}
	Should Be Equal As Integers	${rc}	${result}

Configure Network
	[Documentation]	Configure network for emulated device
	${ip}=	Set Variable	${IP_ADDRESS}/${IP_MASK}
	${stderr}	${rc}=	Execute Command	ip addr add ${ip} dev ${DEVICE}	sudo=True	sudo_password=${PASSWORD}	return_stdout=False	return_stderr=True	return_rc=True
	Run Keyword If	${rc}	Log	${stderr}
	Should Be Equal As Integers	${rc}	0
	Check Device	0

Check Network
	[Documentation]	Send ping to check network for emulated device
	[Arguments]	${result}
	${output}	${stderr}	${rc}=	Execute Command	ping -c ${PING_NUMBER} ${PEER_IP_ADDRESS}	return_stderr=True	return_rc=True
	Log	${output}
	Run Keyword If	${rc}	Log	${stderr}
	Should Be Equal As Integers	${rc}	${result}
