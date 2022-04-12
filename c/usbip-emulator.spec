Name:     usbip-emulator
Version:  1.00
Release:  1%{?dist}
Summary:  USBIP hardware emulator

License:  GPLv2+
URL:      https://github.com/jtornosm/USBIP-Virtual-USB-Device
Source0:  %{name}-%{version}.src.tar.gz

%description
This package contains several usb hadware emulators that could be very useful to test usb devices. 
Generated emulators use the usbip kernel feature.


%global debug_package %{nil}


%prep
%setup -q

%build
make

%install
mkdir -p %{buildroot}/usr/sbin/
install -m 755 hso %{buildroot}/usr/sbin/hso
install -m 755 cdc-ether %{buildroot}/usr/sbin/cdc-ether
install -m 755 bt %{buildroot}/usr/sbin/bt


%files
/usr/sbin/hso
/usr/sbin/cdc-ether
/usr/sbin/bt

%changelog
* Tue Apr 12 2022 Jose Ignacio Tornos Martinez <jtornosm@redhat.co
- Add bt support

* Mon Feb 28 2022 Jose Ignacio Tornos Martinez <jtornosm@redhat.com>
- Initial version of the package
