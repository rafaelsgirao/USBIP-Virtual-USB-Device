*** Settings ***
Documentation	Bluetooth test script

Library		SSHLibrary
Suite Setup	Open Connection And Log In
Suite Teardown	Close All Connections
Resource	test_common.robot

*** Variables ***
${HOST}		192.168.122.252
${USERNAME}	robot_test
${PASSWORD}	robot_test
${ALIAS}	bt_test
${JOURNALCTL_LOG}	/tmp/bt_journalctl.log
${EMULATOR}	bt
${EMULATOR_LOG}	/tmp/bt.log
${TCP_PORT}	3243
${BUS_PORT}	1-1
${BT_NUMBER}	0
${BT_TEXT}	Bluetooth

*** Test Cases ***
Init Bluetooth
	[Documentation]	Init emulated bt device and check 
	Start Emulator
	Load usbip Modules
	Attach usbip
	Sleep	1
	Check Device	0

Turn Off Bluetooth
	[Documentation]	Turn off emulated bt device and check 
	Get BT Number
	Turn Off

Turn On Bluetooth
	[Documentation]	Turn on emulated bt device and check 
	Get BT Number
	Turn On

Release Bluetooth
	[Documentation]	Release emulated bt device and check
	Detach usbip
	Check Device	1
	Kill Emulator

Check logs
	Check Emulator Log
	Check journalctl Log

*** Keywords ***
Check Device
	[Documentation]	Check bt device
	[Arguments]	${result}
	${output}	${stderr}	${rc}=	Execute Command	hciconfig -a	sudo=True	sudo_password=${PASSWORD}	return_stderr=True	return_rc=True
	Log	${output}
	Run Keyword If	${rc}	Log	${stderr}
	Should Be Equal As Integers	${rc}	0
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
		Should Contain	${output}	${BT_TEXT}
	ELSE
		Should Not Contain	${output}	${BT_TEXT}
	END

Get BT Number
	${number}=	Execute Command	rfkill list | grep ${BT_TEXT} | cut -d ":" -f 1
	Should Not Be Empty	${number}
	Log	${number}
	Set test variable	${BT_NUMBER}	${number}

Turn Off
	${stderr}	${rc}=  Execute Command	rfkill block ${BT_NUMBER}	sudo=True	sudo_password=${PASSWORD}	return_stdout=False	return_stderr=True	return_rc=True
	Run Keyword If	${rc}	Log	${stderr}
	Should Be Equal As Integers	${rc}	0
	Sleep	1
	${output}	${stderr}	${rc}=	Execute Command	hciconfig	return_stderr=True	return_rc=True
	Log	${output}
	Run Keyword If	${rc}	Log	${stderr}
	Should Be Equal As Integers	${rc}	${0}
	Should Not Contain	${output}	RUNNING

Turn On
	${stderr}	${rc}=  Execute Command	rfkill unblock ${BT_NUMBER}	sudo=True	sudo_password=${PASSWORD}	return_stdout=False	return_stderr=True	return_rc=True
	Run Keyword If	${rc}	Log	${stderr}
	Should Be Equal As Integers	${rc}	0
	Sleep	1
	${output}	${stderr}	${rc}=	Execute Command	hciconfig	return_stderr=True	return_rc=True
	Log	${output}
	Run Keyword If	${rc}	Log	${stderr}
	Should Be Equal As Integers	${rc}	${0}
	Should Contain	${output}	RUNNING
