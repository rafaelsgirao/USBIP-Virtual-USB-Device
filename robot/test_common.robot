*** Variables ***
${KERNEL_OOPS_PATTERN}  kernel: Oops:
${KERNEL_WARNING_PATTERN}       kernel: WARNING:

*** Keywords ***
Open Connection And Log In
	Open Connection	${HOST}	${ALIAS}
	Login		${USERNAME}	${PASSWORD}

Start Emulator
	[Documentation]	Start Emulator
	[Arguments]	${parameters}=""	${root}=0
	Kill journalctl Logger
	Start Command	journalctl -f > ${JOURNALCTL_LOG}
	Kill Emulator	${root}
	IF	not ${root}
		Start Command	${EMULATOR} -p ${TCP_PORT} -b ${BUS_PORT} ${parameters} &> ${EMULATOR_LOG}
	ELSE
		Start Command	${EMULATOR} -p ${TCP_PORT} -b ${BUS_PORT} ${parameters} &> ${EMULATOR_LOG}	sudo=True	sudo_password=${PASSWORD}
	END
	Sleep	1
	Check Emulator	${root}

Load usbip Modules
	[Documentation]    Load necessary kernel modules to run usbip
	${stderr}	${rc}=             Execute Command	modprobe vhci-hcd	sudo=True	sudo_password=${PASSWORD}	return_stdout=False	return_stderr=True	return_rc=True
	Run Keyword If	${rc}	Log	${stderr}
	Should Be Equal As Integers	${rc}	0

Unload usbip Modules
	[Documentation]    Load necessary kernel modules to run usbip
	${stderr}	${rc}=             Execute Command	modprobe -r vhci-hcd	sudo=True	sudo_password=${PASSWORD}	return_stdout=False	return_stderr=True	return_rc=True
	Run Keyword If	${rc}	Log	${stderr}
	Should Be Equal As Integers	${rc}	0

Attach usbip
	[Documentation]	Attach usbip to emulator
	${output}	${stderr}	${rc}=	Execute Command	usbip --tcp-port ${TCP_PORT} attach -r 127.0.0.1 -b ${BUS_PORT}	sudo=True	sudo_password=${PASSWORD}	return_stderr=True	return_rc=True
	Log 	${output}
	Run Keyword If	${rc}	Log	${stderr}
	Should Be Equal As Integers	${rc}	0

Detach usbip
	[Documentation]	Detach usbip from emulator
	# TODO Get resource identifier if more emulated devices are used
	${output}	${stderr}	${rc}=	Execute Command	usbip detach -p 00	sudo=True	sudo_password=${PASSWORD}	return_stderr=True	return_rc=True
	Log 	${output}
	Run Keyword If	${rc}	Log	${stderr}
	Should Be Equal As Integers	${rc}	0

Kill Process
	[Documentation]	Kill process
	[Arguments]	${parameters}	${process}	${root}=0
	IF	not ${root}
		${rc}=	Execute Command	pkill ${parameters} ${process}  return_stdout=False     return_rc=True
	ELSE
		${rc}=	Execute Command	pkill ${parameters} ${process}	sudo=True	sudo_password=${PASSWORD}	return_stdout=False	return_rc=True
	END
	Should Be Equal As Integers	${rc}	0

Kill Emulator
	[Documentation]	Kill emulator
	[Arguments]	${root}=0
	${rc}=	Execute Command	pgrep -x ${EMULATOR}	return_stdout=False	return_rc=True
	Run Keyword If	not ${rc}	Kill Process	-x	${EMULATOR}	${root}

Kill journalctl Logger
	[Documentation]	Kill journarctl logger
	${rc}=	Execute Command	pgrep -fx "bash -c journalctl -f > ${JOURNALCTL_LOG}"	return_stdout=False	return_rc=True
	Run Keyword If	not ${rc}	Kill Process	-fx	"bash -c journalctl -f > ${JOURNALCTL_LOG}"

Check Emulator
	[Documentation]	Check emulator is running
	[Arguments]	${root}=0
	IF	not ${root}
		${rc}=	Execute Command	pgrep -x ${EMULATOR}	return_stdout=False	return_rc=True
	ELSE
		${rc}=	Execute Command	pgrep -x ${EMULATOR}	sudo=True	sudo_password=${PASSWORD}	return_stdout=False	return_rc=True
	END
	Should Be Equal As Integers	${rc}	0

Check Emulator Log
	[Documentation]	Check emulator logs and search error patterns
	Kill Emulator
	${output}=	Execute Command	cat ${EMULATOR_LOG}
	Log	${output}
	${rc}=	Execute Command	rm ${EMULATOR_LOG}	return_stdout=False	return_rc=True
	Should Be Equal As Integers	${rc}	0	

Check journalctl Log
	[Documentation]	Check journalctl logs and search error patterns
	Kill journalctl Logger
	${output}=	Execute Command	cat ${JOURNALCTL_LOG}
	Log	${output}
	Should Not Contain	${output}	${KERNEL_OOPS_PATTERN}
	Should Not Contain	${output}	${KERNEL_WARNING_PATTERN}
	${rc}=	Execute Command	rm ${JOURNALCTL_LOG}	return_stdout=False	return_rc=True
	Should Be Equal As Integers	${rc}	0
