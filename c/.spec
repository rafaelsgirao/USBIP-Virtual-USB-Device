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
mkdir -p %{buildroot}/usr/local/bin/
install -m 755 hso %{buildroot}/usr/local/bin/hso
install -m 755 cdc-ether %{buildroot}/usr/local/bin/cdc-ether


%files
/usr/local/bin/cdc-ether
/usr/local/bin/hso

%changelog
* Mon Feb 28 2022 José Ignacio Tornos Martínez <jtornosm@redhat.com>
- Initial version of the package
