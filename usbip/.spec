Name:     usbip
Version:  4.18
Release:  4%{?dist}
Summary:  USBIP user-space

License:  GPLv2+
URL:      https://www.kernel.org
Source0:  %{name}-%{version}.src.tar.gz

%description
USBIP allows you to share USB devices over a network.  With USB/IP, you can	
plug a USB device into one computer and use it on a different computer on the
network.
This package contains the user-space tools for USB/IP, both for servers and
clients

%global debug_package %{nil}
%define _unpackaged_files_terminate_build 0

%prep
%setup -q

%build
make compile

%install
%make_install

%files
/usr/local/sbin/*
/usr/local/lib/*.so.*
/usr/local/share/man/man8/*

%changelog
* Mon Feb 28 2022 José Ignacio Tornos Martínez <jtornosm@redhat.com>
- Initial version of the package
