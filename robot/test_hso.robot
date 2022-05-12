*** Settings ***
Documentation	hso test script

Library		SSHLibrary
Suite Setup	Open Connection And Log In
Suite Teardown	Close All Connections
Resource	test_common.robot

*** Variables ***
${HOST}		192.168.122.252
${USERNAME}	robot_test
${PASSWORD}	robot_test
${ALIAS}	hso_test
${JOURNALCTL_LOG}	/tmp/hso_journalctl.log
${EMULATOR}	hso
${EMULATOR_LOG}	/tmp/hso.log
${TCP_PORT}	3241
${BUS_PORT}	1-1
${DEVICE}	hso0
${HSO_TEXT}	Wireless WAN

*** Test Cases ***
Init hso with errors
	[Documentation]	Init emulated hso device with errors and check 
	Start Emulator	-e
	Load usbip Modules
	Attach usbip
	Check Device	1
	Unload usbip Modules
	Kill Emulator

Check logs with errors
	Check Emulator Log
	Check journalctl Log

Init hso
	[Documentation]	Init emulated hso device (no error) and check 
	Start Emulator
	Load usbip Modules
	Attach usbip
	Sleep	1
	Check Device	0

Release hso
	[Documentation]	Release emulated hso device and check
	Detach usbip
	Check Device	1
	Unload usbip Modules
	Kill Emulator

Check logs
	Check Emulator Log
	Check journalctl Log

*** Keywords ***
Check Device
	[Documentation]	Check hso device
	[Arguments]	${result}
	${output}	${stderr}	${rc}=	Execute Command	ip addr show dev ${DEVICE}	sudo=True	sudo_password=${PASSWORD}	return_stderr=True	return_rc=True
	Log	${output}
	Run Keyword If	${rc}	Log	${stderr}
	Should Be Equal As Integers	${rc}	${result}
	IF	not ${result}
        	Should Not Be Empty	${output}
	ELSE
		Should Be Empty	${output}
	END
	${output}	${stderr}	${rc}=	Execute Command	rfkill list	return_stderr=True	return_rc=True
        Log	${output}
        Run Keyword If	${rc}	Log	${stderr}
        Should Be Equal As Integers	${rc}	0
	IF	not ${result}
		Should Contain  ${output}       ${HSO_TEXT}
	ELSE
        	Should Not Contain      ${output}       ${HSO_TEXT}
	END

