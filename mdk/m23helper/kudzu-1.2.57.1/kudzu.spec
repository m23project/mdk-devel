Name: kudzu
Version: 1.2.57.1
Release: 1
License: GPL
Summary: The Red Hat Linux hardware probing tool.
Group: Applications/System
URL: http://fedora.redhat.com/projects/additional-projects/kudzu/
Source: kudzu-%{PACKAGE_VERSION}.tar.gz
Obsoletes: rhs-hwdiag setconsole
Prereq: chkconfig, modutils >= 2.3.11-5, /etc/init.d
Requires: pam >= 0.74-17
Conflicts: netconfig < 0.8.18, kernel < 2.6.13, initscripts < 8.40-1
Requires: hwdata >= 0.169-1, hal >= 0.2.96
BuildPrereq: pciutils-devel >= 2.2.3-4, python-devel python gettext
BuildRoot: %{_tmppath}/%{name}-root

%description
Kudzu is a hardware probing tool run at system boot time to determine
what hardware has been added or removed from the system.

%package devel
Summary: Development files needed for hardware probing using kudzu.
Group: Development/Libraries
Requires: pciutils-devel

%description devel
The kudzu-devel package contains the libkudzu library, which is used
for hardware probing and configuration.

%prep

%setup

%build
ln -s `pwd` kudzu

make RPM_OPT_FLAGS="%{optflags} -I." all kudzu

%install
rm -rf $RPM_BUILD_ROOT
make install install-program DESTDIR=$RPM_BUILD_ROOT libdir=$RPM_BUILD_ROOT/%{_libdir}

install -m 755 fix-mouse-psaux $RPM_BUILD_ROOT/%{_sbindir}

%find_lang %{name}

%clean
rm -rf $RPM_BUILD_ROOT

%post
chkconfig --add kudzu

%preun
if [ $1 = 0 ]; then
	chkconfig --del kudzu
fi

%files -f %{name}.lang
%defattr(-,root,root)
%doc README hwconf-description
/sbin/kudzu
%{_sbindir}/kudzu
%{_sbindir}/fix-mouse-psaux
%{_mandir}/man8/*
%config(noreplace) /etc/sysconfig/kudzu
%config /etc/rc.d/init.d/kudzu
%{_libdir}/python*/site-packages/*

%Files devel
%defattr(-,root,root)
%{_libdir}/libkudzu.a
%{_libdir}/libkudzu_loader.a
%{_includedir}/kudzu

%changelog
* Fri Sep 22 2006 Bill Nottingham <notting@redhat.com> - 1.2.57.1-1
- recognize scsi type 14 as disk (#207295)

* Thu Sep 21 2006 Bill Nottingham <notting@redhat.com> - 1.2.56-1
- hack: for some ids, return both ata_piix and ahci (#195779)
- fix section in man page (#207469)

* Tue Sep 19 2006 Bill Nottingham <notting@redhat.com> - 1.2.55-1
- don't remove lock file on stop, so that we only run once on boot
  (ideally, move to rcS.d) (#184944)

* Fri Sep 15 2006 Bill Nottingham <notting@redhat.com> - 1.2.54-1
- write network configs on device add, to make sure names stay
  consistent with udev (#197984)

* Tue Sep  5 2006 Bill Nottingham <notting@redhat.com> - 1.2.53-1
- don't build/ship module_upgrade

* Tue Aug 29 2006 Bill Nottingham <notting@redhat.com> - 1.2.52-1
- fix sg crash
- close fd leaks

* Mon Aug 28 2006 Bill Nottingham <notting@redhat.com> - 1.2.50-1
- fix xenfb probe some more
- clean up some debugging cruft

* Sun Aug 27 2006 Bill Nottingham <notting@redhat.com> - 1.2.48-1
- remove socket code, no longer used (and a bad hack to begin with)
- fix xenfb probe
- various cleanups

* Fri Aug 25 2006 Jeremy Katz <katzj@redhat.com> - 1.2.47-1
- Fix the Xen console stuff to work properly with the PV framebuffer too
- Add support for probing xenfb as a video device

* Thu Aug 24 2006 Bill Nottingham <notting@redhat.com>
- use the sysfs serial list rather than iterating over ttyS0-3 (#64899, #195635)
  - ergo, remove pciserial code

* Thu Aug 24 2006 Jeremy Katz <katzj@redhat.com> - 1.2.46-1
- Xen console support

* Thu Aug 24 2006 Bill Nottingham <notting@redhat.com> - 1.2.45-1
- various cleanups:
  - don't sed the initscript on s390
  - don't explicitly configure parport_serial
  - fix X configuration

* Thu Aug 24 2006 Bill Nottingham <notting@redhat.com> - 1.2.44-1
- remove debugging noise (#203909)

* Thu Aug 17 2006 Bill Nottingham <notting@redhat.com> - 1.2.43-1
- fix the fix for #195934

* Wed Aug  9 2006 Bill Nottingham <notting@redhat.com> - 1.2.42-1
- always insert aliases at head of list (<ajackson@redhat.com>)
- don't parse videoaliases that don't end in .xinf (<ajackson@redhat.com>)
- fix ppc build

* Wed Aug  9 2006 Peter Jones <pjones@redhat.com>
- make pci storage classes for "ATA" and "SATA" work

* Fri Jul 28 2006 Bill Nottingham <notting@redhat.com> - 1.2.41-1
- disable DDC probe; let X sort it out (fixes #189602, #189131,
  #186538, #177456, #176869)
 
* Tue Jul 25 2006 Bill Nottingham <notting@redhat.com> - 1.2.40-1
- ignore comments in videoaliases (<clumens@redhat.com>)
- grab mac address for ibmveth devices (#195934, in theory)

* Mon Jul 24 2006 Jeremy Katz <katzj@redhat.com> - 1.2.39-1
- fix probing of xenblk as a module

* Thu Jun 15 2006 Jeremy Katz <katzj@redhat.com> - 1.2.38-1
- more s390 segfault fixing

* Wed Jun 14 2006 Jeremy Katz <katzj@redhat.com> - 1.2.37-1
- fix s390 segfault

* Mon May 22 2006 Bill Nottingham <notting@redhat.com> - 1.2.36-1
- add asix to usb whitelisting code

* Mon May  8 2006 Bill Nottingham <notting@redhat.com> - 1.2.35-1
- fix a module_upgrade segfault (#190938)

* Tue Mar 14 2006 Bill Nottingham <notting@redhat.com> - 1.2.34.3-1
- fix scsi probe to not return devices of the wrong class

* Mon Mar 13 2006 Bill Nottingham <notting@redhat.com>
- fix fd leak (#185283)

* Thu Mar  9 2006 Bill Nottingham <notting@redhat.com> - 1.2.34.2-1
- special-case xen to not use vm86 (works around #179013)
-
* Tue Mar  7 2006 Bill Nottingham <notting@redhat.com> - 1.2.34.1-1
- switch at runtime between vm86 and x86emu on i386. Fixes vbe/ddc on
  Xen and i386-on-x86_64
  
* Mon Mar  6 2006 Bill Nottingham <notting@redhat.com> - 1.2.34-1
- silence some error messages

* Tue Feb 28 2006 Jeremy Katz <katzj@redhat.com> - 1.2.33-1
- fix i2o again (#182522)

* Wed Feb 22 2006 Bill Nottingham <notting@redhat.com> - 1.2.32-1
- fix potential segfault with USB network devices (#182006)

* Tue Feb 21 2006 Bill Nottingham <notting@redhat.com> - 1.2.31-1
- further x86emu-related fixes (#181467)

* Wed Feb 15 2006 Bill Nottingham <notting@redhat.com> - 1.2.30-1
- revert changes for video probing classes - some correct devices are
  0380

* Tue Feb 14 2006 Bill Nottingham <notting@redhat.com> - 1.2.29-1
- silence!

* Mon Feb 13 2006 Bill Nottingham <notting@redhat.com> - 1.2.28-1
- vbe/edid probing on x86_64 via x86emu - thanks to Matthew Garrett
  & Tollef Fog Heen

* Mon Feb 13 2006 Bill Nottingham <notting@redhat.com> - 1.2.27-1
- restrict video devices to PCI class 0300, correctly (should solve #176978)

* Mon Feb 13 2006 Bill Nottingham <notting@redhat.com> - 1.2.26-1
- restrict video devices to PCI class 0300 (should solve #176978)
- translation updates

* Tue Feb  7 2006 Bill Nottingham <notting@redhat.com> - 1.2.25-1
- in installer environment, sg isn't loaded, so there are deviceless
  scsi devs (#180378)

* Mon Jan 30 2006 Bill Nottingham <notting@redhat.com> - 1.2.24-1
- fix xenblk/i2o sysfs probing

* Wed Jan 25 2006 Jeremy Katz <katzj@redhat.com> - 1.2.23-1
- fix nvidia segfault

* Wed Jan 25 2006 Jeremy Katz <katzj@redhat.com> - 1.2.22-1
- fix xen device probing when modules aren't loaded

* Tue Jan 24 2006 Bill Nottingham <notting@redhat.com> - 1.2.21-1
- more forcedeth shenaningans (#178557)

* Mon Jan 23 2006 Bill Nottingham <notting@redhat.com> - 1.2.20-1
- switch scsi probe over to sysfs

* Fri Jan 20 2006 Bill Nottingham <notting@redhat.com>
- fix potential segfault in DDC probing code

* Mon Jan 16 2006 Bill Nottingham <notting@redhat.com> - 1.2.19-1
- don't configure (or unconfigure) storage adapters (#177740)

* Tue Jan 10 2006 Bill Nottingham <notting@redhat.com> - 1.2.18-1
- add missing fchdir in pcmcia code
- fix segfault if pcmcia network devices are found before their drivers
  are loaded (#174341)

* Sun Jan  1 2006 Bill Nottingham <notting@redhat.com> - 1.2.17-1
- handle pcilib string returns (#176490, #176724)

* Wed Dec 14 2005 Bill Nottingham <notting@redhat.com> - 1.2.16-1
- revert LRMI changes (#175677)

* Tue Dec 13 2005 Bill Nottingham <notting@redhat.com> - 1.2.15-1
- use ACPI EDID info if available

* Fri Dec  9 2005 Bill Nottingham <notting@redhat.com> - 1.2.14-1
- add support for mambo virtual devices (#173307, <dwmw2@redhat.com>)

* Wed Nov 16 2005 Bill Nottingham <notting@redhat.com> - 1.2.13-1
- integrate patch, bump version

* Wed Nov 16 2005 Dave Jones <davej@redhat.com> - 1.2.12-2
- Hopefully fix ddcprobe to work with > 2.6.14 kernels.

* Tue Nov 15 2005 Bill Nottingham <notting@redhat.com> - 1.2.12-1
- allow multiple files for video aliases under /usr/share/hwdata/videoaliases

* Wed Nov  9 2005 Jeremy Katz <katzj@redhat.com> - 1.2.11-1
- fix xen driver names

* Fri Oct 28 2005 Jeremy Katz <katzj@redhat.com> - 1.2.10-1
- add some basic xen device probing

* Fri Sep 23 2005 Bill Nottingham <notting@redhat.com> 1.2.9-1
- move kudzu to /sbin (since we no longer use newt (#74736))
- don't congfigure usb/firewire controllers, modems, scanners
  in kudzu program (as such configurations aren't used)

* Thu Sep 22 2005 Bill Nottingham <notting@redhat.com> 1.2.8-1
- fix crash in sortNetDevices (#169003)

* Mon Sep 19 2005 Bill Nottingham <notting@redhat.com> 1.2.7-1
- fix fbProbe to work with X drivers, not card entries
- fix crash in matchNetDevices (#168689)

* Sun Sep 18 2005 Bill Nottingham <notting@redhat.com> 1.2.5-1
- fix module_upgrade

* Fri Sep 16 2005 Bill Nottingham <notting@redhat.com> 1.2.4-1
- remove obsolete updfstab code
- ABI change: drivers are no longer set to unknown/ignore/disabled;
  they are just left as NULL
- remove support for loading modules; it's not used by any library
  consumers
- remove support for system-config-mouse, as it's no longer shipped
- read hwaddrs for network devices from sysfs, not ethtool (and
  conflict with older kernels that don't support that)

* Wed Sep 14 2005 Bill Nottingham <notting@redhat.com> 1.2.3-1
- port pcmcia probe to new model

* Fri Sep  9 2005 Bill Nottingham <notting@redhat.com> 1.2.2-1
- fix usb device list double-free
- fix passing of filenames to (pci|usb)ReadDrivers()

* Thu Sep  8 2005 Bill Nottingham <notting@redhat.com> 1.2.1-1
- switch pci, usb probing to use modules.alias
- switch usb probe to use sysfs
- remove pcitable support
- X drivers are now the video.xdriver field of CLASS_VIDEO
  (framebuffer drivers will be returned if they match)

* Tue Aug 30 2005 Bill Nottingham <notting@redhat.com> 1.1.122-1
- don't rely on pcimap for 8139too/8139cp; hardcode the logic (#157783)

* Mon Aug 22 2005 Bill Nottingham <notting@redhat.com> 1.1.121-1
- make sure kernel version is always initialized (fixes python bindings)

* Fri Aug 19 2005 Bill Nottingham <notting@redhat.com> 1.1.120-1
- fix macio overzealous snd-powermac probe (#166011, <dwmw2@redhat.com>)
- fix overriding of kernel version
  - allow using that in module_upgrade
- remove usb, firewire special cases in pci probing

* Tue Jul 26 2005 Bill Nottingham <notting@redhat.com> 1.1.119-1
- make sure changing network devices are properly caught (#141338)

* Fri Jul 22 2005 Bill Nottingham <notting@redhat.com> 1.1.118-1
- make sure ISAPNP devices don't end up as CLASS_UNSPEC (#163949)

* Thu Jun 16 2005 Bill Nottingham <notting@redhat.com>
- fix usb - class/subclass/protocol are hex, not decimal (#160450)

* Wed May 18 2005 Bill Nottingham <notting@redhat.com> 1.1.117-1
- probe ACPI PnP devices as well (#146405)
- tweak synaptics detection for ALPS (#158062, <pnasrat@redhat.com>)

* Fri Apr 29 2005 Bill Nottingham <notting@redhat.com> 1.1.116-1
- fix nforce4 fix
- some readlink() usage fixes

* Wed Apr 27 2005 Bill Nottingham <notting@redhat.com> 1.1.115-1
- hack for nForce4 (#153176)
- fix uninitialized memory use in pci probe

* Mon Apr 25 2005 Bill Nottingham <notting@redhat.com> 1.1.114-1
- fix snd-powermac configuration (#151591)

* Fri Apr 22 2005 Bill Nottingham <notting@redhat.com> 1.1.113-1
- fix some potential segfaults if the files read are empty (#145998)
- don't write install lines for sound cards (not useful with udev),
  add a 'index' parameter for the acutal module, not just the alias
  (#133759)
- support modules.pcimap class/class_mask, and generic entries too (#146213)

* Sat Apr 16 2005 Bill Nottingham <notting@redhat.com> 1.1.112-1
- make up some fan devices for mac things that can only be found in
  the device-tree (#151661)
- fix crash in vio.c if the viocd file is empty (#154905)
- fix firewire crash (#153987, <danieldk@pobox.com>)
- add PCI class for HD audio

* Thu Mar  3 2005 Bill Nottingham <notting@redhat.com> 1.1.111-1
- handle -q/--quiet

* Wed Mar  2 2005 Bill Nottingham <notting@redhat.com> 1.1.110-1
- fix reading of modules.pcimap when /usr/share/hwdata isn't there
  (#150123, <dlehman@redhat.com>)
- support fddiX device naming (#109689)

* Thu Feb 24 2005 Bill Nottingham <notting@redhat.com> 1.1.109-1
- use synaptics for ALPS touchpad (#149619, <pnasrat@redhat.com>)

* Fri Feb 18 2005 Bill Nottingham <notting@redhat.com> 1.1.108-1
- add patch for detecting macio bmac devices (<dwmw2@redhat.com>)

* Tue Feb 01 2005 Karsten Hopp <karsten@redhat.de> 1.1.107-1 
- fix detection of mainframe lcs devices

* Mon Jan  3 2005 Bill Nottingham <notting@redhat.com> - 1.1.106-1
- fix a fd leak in the sx8/i2o probe
- add a method for reading probed devices from a socket
- attempt to use that method by default
- don't tweak network configurations for now

* Fri Dec 17 2004 Bill Nottingham <notting@redhat.com> - 1.1.104-1
- don't configure/unconfigure printers
- don't probe serial bus by default in /usr/sbin/kudzu
- remove the UI - run without user interaction

* Thu Dec 16 2004 Bill Nottingham <notting@redhat.com> - 1.1.103-1
- tweak network device detection algorithm (#141338)

* Tue Nov 30 2004 Bill Nottingham <notting@redhat.com> - 1.1.101-1
- clear out driver on new usb interfaces (#141358)

* Wed Nov 24 2004 Bill Nottingham <notting@redhat.com> - 1.1.100-1
- clear out device on new usb interfaces (#130805)

* Tue Nov 23 2004 Bill Nottingham <notting@redhat.com> - 1.1.99-1
- speed up pci probing

* Tue Nov 23 2004 Jeremy Katz <katzj@redhat.com> - 1.1.98-1
- fix my undiet-ifying

* Mon Nov 22 2004 Jeremy Katz <katzj@redhat.com> - 1.1.97-1
- don't use dietlibc on x86 anymore

* Mon Nov 22 2004 Bill Nottingham <notting@redhat.com> - 1.1.96-1
- replace significantly suboptimal module availability algorithm

* Tue Oct 12 2004 Bill Nottingham <notting@redhat.com> - 1.1.95-1
- fix potential segfault on odd USB controllers (#135450)

* Tue Oct 12 2004 Bill Nottingham <notting@redhat.com> - 1.1.94-1
- add a quick hack to avoid warning (#129181)

* Fri Oct  8 2004 Bill Nottingham <notting@redhat.com> - 1.1.93-1
- fix crash when there's no modprobe.conf/modules.conf

* Fri Oct  1 2004 Bill Nottingham <notting@redhat.com> - 1.1.92-1
- add mapping for wacom (#132738)

* Wed Sep 29 2004 Bill Nottingham <notting@redhat.com> - 1.1.91-1
- fix file leak (#133373, <mitr@redhat.com>)
- add speaker support (fixes #134187 in the process)
- have better ISAPnP descriptions

* Fri Sep 17 2004 Bill Nottingham <notting@redhat.com> - 1.1.90-1
- migrate OSS mixer save/restore lines on upgrade from OSS modules
- write 'options snd-card-%d index=%d' for ALSA
- clean up sound card removal code

* Thu Sep  9 2004 Bill Nottingham <notting@redhat.com> - 1.1.89-1
- fix searching when it returns descriptionless keys

* Wed Sep  8 2004 Bill Nottingham <notting@redhat.com> - 1.1.88-1
- fix pcitable parsing bug introduced in 1.1.86-1

* Tue Sep  7 2004 Jeremy Katz <katzj@redhat.com> - 1.1.87-1
- add probing of ibmveth and ibmvscsic for POWER5 boxes

* Fri Sep  3 2004 Bill Nottingham <notting@redhat.com> 1.1.86-1
- read pci device descriptions from pci.ids, not pcitable

* Thu Sep  2 2004 Bill Nottingham <notting@redhat.com> 1.1.85-1
- use new pci bits for getting device info (#115522)

* Tue Aug 31 2004 Bill Nottingham <notting@redhat.com> 1.1.84-1
- fix loader segfault (#131375)
- updated i2o probing (<katzj@redhat.com>)

* Mon Aug 30 2004 Karsten Hopp <karsten@redhat.de> 1.1.83-1 
- fix detection of ESCON devices (mainframe)
- change QETH and Hipersockets detection to use /sys/class/net (mainframe)
- add detection of lcs(osa) and lcs(tokenring) (mainframe)

* Fri Aug 27 2004 Bill Nottingham <notting@redhat.com> - 1.1.82-1
- tweak net device matching algorithm

* Wed Aug 26 2004 Karsten Hopp <karsten@redhat.de> 1.1.81-1 
- add iucv detection (mainframe)

* Thu Aug 26 2004 Bill Nottingham <notting@redhat.com> - 1.1.80-1
- add PROBE_NOLOAD option for probing without loading modules

* Fri Aug 13 2004 Bill Nottingham <notting@redhat.com> - 1.1.79-1
- remove updfstab from the initscript too

* Fri Aug 13 2004 Bill Nottingham <notting@redhat.com> - 1.1.78-1
- remove updfstab in favor of HAL's fstab-sync

* Wed Aug 11 2004 Bill Nottingham <notting@redhat.com> - 1.1.77-1
- use vt100-nav for serial and related consoles (#129659)

* Wed Jul 28 2004 Bill Nottingham <notting@redhat.com> - 1.1.76-1
- check that consoles that respond to TIOCGSERIAL actually map to ttySX
  (#120880)

* Mon Jul 26 2004 Bill Nottingham <notting@redhat.com> - 1.1.75-1
- Altix and HVSI console support

* Thu Jul 22 2004 Jeremy Katz <katzj@redhat.com> - 1.1.74-1
- fix another PPC segfault

* Wed Jul 14 2004 Bill Nottingham <notting@redhat.com> - 1.1.73-1
- fix ddc probe on ppc (#127827)
- make USB probe ignore auxillary interfaces (#79278, <bnocera@redhat.com>)

* Tue Jun 29 2004 Jeremy Katz <katzj@redhat.com> - 1.1.72-1
- don't segfault with a zero-byte modules.usbmap

* Wed Jun 23 2004 Jeremy Katz <katzj@redhat.com> - 1.1.71-1
- use new sysfs bits for vio probing on iSeries

* Fri Jun 18 2004 Jeremy Katz <katzj@redhat.com> - 1.1.70-1
- fix build with gcc 3.4

* Thu Jun  3 2004 Bill Nottingham <notting@redhat.com> - 1.1.69-1
- look for the right X server (#125169, <alexl@redhat.com>)
- configure X even in non-interactive mode

* Mon May 24 2004 Bill Nottingham <notting@redhat.com> - 1.1.68-1
- fix checking of modules loaded which have a - in their name as 
  /proc/modules will contain an _ instead, this time for the !loader
  case (#122983, at least)

* Fri May 21 2004 Jeremy Katz <katzj@redhat.com> - 1.1.67-1
- look for module.usbmap under /modules also for anaconda usage

* Wed May 19 2004 Bill Nottingham <notting@redhat.com> 1.1.66-1
- MacIO fixes (#115286, <alex.kiernan@thus.net>)

* Thu May 13 2004 Karsten Hopp <karsten@redhat.de> 1.1.65-1 
- add CTC and Escon detection (mainframe)

* Tue May 11 2004 Karsten Hopp <karsten@redhat.de> 1.1.64-1 
- change QETH module name back, newer kernels have reverted the
  name change

* Mon May 10 2004 Jeremy Katz <katzj@redhat.com> - 1.1.63-1
- minor fix for viodasd probing

* Fri May  7 2004 Jeremy Katz <katzj@redhat.com> - 1.1.62-1
- add i2o probing code from Markus Lidel

* Thu May  6 2004 Karsten Hopp <karsten@redhat.com> 1.1.61-1
- fix QETH module name
- add HiperSockets 
- fix DASD detection

* Wed May  5 2004 Jeremy Katz <katzj@redhat.com> 1.1.60-1
- fix checking of modules loaded which have a - in their name as 
  /proc/modules will contain an _ instead (#121955, #120289, #120360)

* Mon May  3 2004 Bill Nottingham <notting@redhat.com> - 1.1.59-1
- shut up modprobe (#120410)
- re-enable the mouse config code (#121139, #121487)

* Thu Apr 29 2004 Jeremy Katz <katzj@redhat.com> - 1.1.58-1
- fix libkudzu_loader for ppc

* Mon Apr 19 2004 Jeremy Katz <katzj@redhat.com> - 1.1.57-1
- add probing for VIO bus for iSeries
- add probing for S390 bus for s390

* Thu Apr 15 2004 Bill Nottingham <notting@redhat.com> - 1.1.56-1
- move updfstab.conf to hwdata
- fix sound aliases to be snd-card-X

* Thu Apr  1 2004 Bill Nottingham <notting@redhat.com> - 1.1.54-1
- fix overrun in usb code (#119654)
- fix use-after-free in network code (#119655, <alex.kiernan@thus.net>)

* Wed Mar 24 2004 Bill Nottingham <notting@redhat.com>
- mouse configuration fixes

* Wed Mar 17 2004 Bill Nottingham <notting@redhat.com> - 1.1.53-1
- hack: add mouse-cleaner-upper here

* Fri Mar 12 2004 Bill Nottingham <notting@redhat.com> - 1.1.52-1
- fix libkudzu_loader

* Thu Mar 11 2004 Bill Nottingham <notting@redhat.com> - 1.1.51-1
- add a PROBE_LOADED for installer use
- remove ide-scsi cdwriter hacks from updfstab

* Wed Mar 10 2004 Jeremy Katz <katzj@redhat.com> - 1.1.50-1
- fix loader segfault

* Wed Mar 10 2004 Bill Nottingham <notting@redhat.com> 1.1.49-1
- do some more munging in the loader-specific code, for cards
  without ethtool support
- minimal port of the ieee1394 sbp2 probe to 2.6

* Thu Mar  4 2004 Bill Nottingham <notting@redhat.com> 1.1.48-1
- fix module_upgrade

* Mon Mar  1 2004 Bill Nottingham <notting@redhat.com> 1.1.47-1
- fix dac960 probe (#116126, <heinz@auto.tuwien.ac.at>)

* Wed Feb 25 2004 Bill Nottingham <notting@redhat.com> 1.1.46-1
- write more correct audio lines for 2.6
- fix pcmcia probe to only report the class you asked for

* Fri Feb 20 2004 Bill Nottingham <notting@redhat.com> 1.1.45-1
- fix various bogosities in new and improved PS/2 probe

* Wed Feb 11 2004 Bill Nottingham <notting@redhat.com> 1.1.44-1
- new and improved PS/2 probe - requires a 2.6 kernel

* Fri Feb  6 2004 Bill Nottingham <notting@redhat.com> 1.1.43-1
- add patch for smartarray/dac960 devices (<katzj@redhat.com>)

* Wed Feb  4 2004 Bill Nottingham <notting@redhat.com> 1.1.42-1
- fix segfault on CLASS_NETWORK devices with no device set (#106332)
- fix various network device naming snafus (#114611, #113418)

* Thu Jan 29 2004 Bill Nottingham <notting@redhat.com> 1.1.41-1
- switch some behaviors on 2.4 vs 2.6

* Fri Dec  5 2003 Jeremy Katz <katzj@redhat.com> 1.1.40-1
- write out install/remove instead of post-install/pre-remove for modprobe.conf

* Fri Nov 21 2003 Bill Nottingham <notting@redhat.com> 1.1.39-1
- pci domain support

* Mon Nov 17 2003 Jeremy Katz <katzj@redhat.com> 1.1.38-1
- fix scsi probing for 2.6 kernel

* Thu Nov  6 2003 Jeremy Katz <katzj@redhat.com> 1.1.37-1
- rebuild for python 2.3

* Fri Oct 31 2003 Bill Nottingham <notting@redhat.com> 1.1.36-1
- only reset rhgb details screen if we turned it on (#108712)

* Wed Oct 22 2003 Bill Nottingham <notting@redhat.com> 1.1.35-1
- take out the check for local X display (#107679)
- a couple of rhgb interaction tweaks

* Fri Oct 17 2003 Bill Nottingham <notting@redhat.com> 1.1.34-1
- switch to new scsi device id code (#102136, 107350)
- fix scsi tape enumeration (#107361)

* Fri Oct 10 2003 Bill Nottingham <notting@redhat.com> 1.1.33-1
- add a diskonkey device (#105939, <bnocera@redhat.com>)

* Wed Oct  8 2003 Bill Nottingham <notting@redhat.com> 1.1.32-1
- ignoremulti option and corresponding iPod support (#98881, <bnocera@redhat.com>)
- add a NEC USB floppy (#97681, <Diego.SantaCruz@epfl.ch>)
- add another flash entry (#101990, <mccannwj@pha.jhu.edu>
- don't probe PS/2 mice if rhgb appears to be running

* Fri Oct  3 2003 Bill Nottingham <notting@redhat.com> 1.1.31-1
- tweak RadeonIGP exclude list to be less exclusive

* Thu Oct  2 2003 Bill Nottingham <notting@redhat.com> 1.1.30-1
- resurrect PCI disabled probe for video cards (#91265, #106030)
- don't probe PS/2 mice if we appear to be running on a local X display

* Wed Sep 24 2003 Bill Nottingham <notting@redhat.com> 1.1.21-1
- fix clashing 'serial' consoles (#104851)

* Wed Aug 27 2003 Bill Nottingham <notting@redhat.com> 1.1.20-1
- export CLASS_IDE in the python module

* Mon Aug 25 2003 Bill Nottingham <notting@redhat.com> 1.1.19-1
- fix a bug that could misorder ethernet devices

* Fri Aug 22 2003 Bill Nottingham <notting@redhat.com> 1.1.18-1
- set up hvc0 on pSeries (#98007)

* Mon Aug 18 2003 Bill Nottingham <notting@redhat.com> 1.1.17-1
- fix segfault, and don't call matchNetDevices() so often (#102617)

* Fri Aug 15 2003 Bill Nottingham <notting@redhat.com> 1.1.16-1
- more changes/fixes to guarantee uniqueness of network device names
- use this for configuration/deconfiguration (#61169)
- various other networking-related fixes (#101488, #99269)

* Tue Aug  5 2003 Bill Nottingham <notting@redhat.com> 1.1.15-1
- viocd probing (#89232)
- read /etc/sysconfig/network-scripts/ifcfg-* for device names (#99269)

* Thu Jul 17 2003 Jeremy Katz <katzj@redhat.com> 1.1.13-1
- fix another segfault (#99317)

* Tue Jul 15 2003 Bill Nottingham <notting@redhat.com> 1.1.12-1
- fix segfault (#99176)

* Fri Jul 11 2003 Bill Nottingham <notting@redhat.com> 1.1.11-1
- remove debugging code. duh.

* Thu Jul 10 2003 Bill Nottingham <notting@redhat.com> 1.1.10-1
- fix bug that would cause us to lose track of some network devices

* Thu Jul  3 2003 Bill Nottingham <notting@redhat.com> 1.1.9-1
- associate ethernet hardware address with PCI, PCMCIA, USB ethernet
  devices

* Tue Jun 24 2003 Bill Nottingham <notting@redhat.com> 1.1.7-1
- fix updfstab to not do bizarre things when called in parallel (#89229)

* Thu Jun 19 2003 Bill Nottingham <notting@redhat.com> 1.1.6-1
- fix some of the python bindings

* Mon Jun  9 2003 Bill Nottingham <notting@redhat.com> 1.1.5-1
- probe IDE controllers, for those driven by separate modules (SATA)

* Wed May 21 2003 Brent Fox <bfox@redhat.com> 1.1.4-1.1
- replace call to kbdconfig with redhat-config-keyboard (bug #87919)

* Thu Apr 17 2003 Bill Nottingham <notting@redhat.com> 1.1.3-1
- fix severe blowing of the stack

* Wed Apr 16 2003 Bill Nottingham <notting@redhat.com> 1.1.2-1
- fix module_upgrade

* Wed Mar 26 2003 Bill Nottingham <notting@redhat.com> 1.1.1-1
- fix asignment of class in readDevice()

* Mon Mar 17 2003 Bill Nottingham <notting@redhat.com> 1.1.0-1
- change semantics of class enumeration; now treated bitwise, like bus
- make BUS_UNSPEC and CLASS_UNSPEC ~0 instead of 0
- change field name from 'class' to 'type' for C++ happiness (#26463)

* Tue Feb 25 2003 Bill Nottingham <notting@redhat.com> 0.99.99-1
- fix syntax error (#85127)

* Thu Feb 20 2003 Bill Nottingham <notting@redhat.com> 0.99.98-1
- fix segfault in updfstab --skipprobe by weeding out non
  ide/scsi/misc/firewire devices

* Wed Feb 19 2003 Bill Nottingham <notting@redhat.com> 0.99.97-1
- don't say PS/2 mice are removed in safe mode (#80662)
- don't configure USB mice; they're always configured (#84047)
- fix PCMCIA ranges (<katzj@redhat.com>)
- set LANG, not LC_MESSAGES, for CJK in init script

* Thu Feb 13 2003 Bill Nottingham <notting@redhat.com> 0.99.96-1
- add support for another USB floppy drive to updfstab (#70282)
- fix removal of wrong device in updfstab on USB device removal (#77382)

* Thu Feb 13 2003 Elliot Lee <sopwith@redhat.com> 0.99.95-1
- Add ppc64 to Makefile

* Tue Feb 11 2003 Bill Nottingham <notting@redhat.com> 0.99.94-1
- MacIO, ADB, and ppc DDC probing (<dburcaw@terrasoftsolutions.com>)
- tweak serial probe
- fix silly initscript typo

* Fri Jan 31 2003 Bill Nottingham <notting@redhat.com> 0.99.92-1
- fix segfault (#83255)

* Wed Jan 29 2003 Bill Nottingham <notting@redhat.com> 0.99.91-1
- support in updfstab for linking /dev/cdwriter to /dev/sg*

* Thu Jan 23 2003 Bill Nottingham <notting@redhat.com> 0.99.90-1
- don't show CJK on console (#82537)
- fix handling of null strings in kudzumodule

* Mon Jan 20 2003 Bill Nottingham <notting@redhat.com> 0.99.89-1
- really fix the broken ps2 probe

* Thu Jan 16 2003 Bill Nottingham <notting@redhat.com> 0.99.88-1
- ps2 probe fixing

* Fri Jan 10 2003 Bill Nottingham <notting@redhat.com> 0.99.87-1
- add -k to specify kernel version to look for modules for
- usb class -> kudzu class mapping tweaks

* Mon Jan  6 2003 Bill Nottingham <notting@redhat.com> 0.99.86-1
- PCMCIA probing support
- PCMCIA fixes

* Tue Dec  3 2002 Bill Nottingham <notting@redhat.com> 0.99.83-1
- switch floppy probing back

* Tue Nov 26 2002 Bill Nottingham <notting@redhat.com> 0.99.82-1
- write "udf,iso9660" in updfstab

* Fri Nov 22 2002 Bill Nottingham <notting@redhat.com> 0.99.81-1
- handle compressed modules

* Thu Nov 14 2002 Preston Brown <pbrown@redhat.com> 0.99.80-1
- speed up floppy probe.

* Wed Nov 13 2002 Bill Nottingham <notting@redhat.com> 0.99.79-1
- fix broken ps2 probe
- remove check for X in ps2 code

* Wed Nov 13 2002 Preston Brown <pbrown@redhat.com> 0.99.77-1
- boot-time optimizations and speedups

* Tue Nov 12 2002 Bill Nottingham <notting@redhat.com> 0.99.76-1
- mouse driver tweakage

* Mon Nov  4 2002 Bill Nottingham <notting@redhat.com> 0.99.75-1
- deal with switched network driver mappings

* Tue Sep  3 2002 Bill Nottingham <notting@redhat.com> 0.99.69-1
- don't have mouseconfig modify X configs for USB mice (#69581)

* Mon Sep  2 2002 Bill Nottingham <notting@redhat.com> 0.99.68-1
- fix parallel device naming
- fix printer config (<twaugh@redhat.com>)

* Fri Aug 23 2002 Jeremy Katz <katzj@redhat.com> 0.99.66-1
- build libkudzu_loader with diet on x86 to avoid conflicting ideas about
  dirent in the loader

* Wed Aug 21 2002 Bill Nottingham <notting@redhat.com> 0.99.65-1
- fix duplicate modules.conf entries, and other weirdness (#68905, #68044)

* Thu Aug 15 2002 Bill Nottingham <notting@redhat.com> 0.99.64-2
- rebuild against new newt

* Tue Aug 13 2002 Bill Nottingham <notting@redhat.com> 0.99.64-1
- fix firewire probe
- fix firewire mapping in python module

* Mon Jul 29 2002 Bill Nottingham <notting@redhat.com> 0.99.63-4
- add missing header

* Wed Jul 24 2002 Bill Nottingham <notting@redhat.com> 0.99.63-1
- USB printer config support

* Tue Jul 23 2002 Bill Nottingham <notting@redhat.com> 0.99.62-1
- printer config support

* Mon Jul 22 2002 Bill Nottingham <notting@redhat.com> 0.99.61-1
- kill Xconfigurator support, add redhat-config-xfree86 support

* Sun Jul 21 2002 Bill Nottingham <notting@redhat.com> 0.99.60-1
- firewire controller support (#65386, <hjl@gnu.org>)
- minimal firewire bus probing

* Wed Jul 17 2002 Bill Nottingham <notting@redhat.com> 0.99.59-1
- fix fix for #66652
- fix serial console support

* Thu Jul 11 2002 Bill Nottingham <notting@redhat.com> 0.99.57-1
- fix assorted brokenness in the DDC probing

* Thu Jul 11 2002 Erik Troan <ewt@redhat.com> 0.99.56-1
- collapsed jaz/zip entries into consistent /mnt/jaz, /mnt/zip mount points

* Sun Jul 07 2002 Erik Troan <ewt@redhat.com>
- added CAMERA search string for updfstab (Minolta S304, at least)

* Thu Jun 27 2002 Bill Nottingham <notting@redhat.com> 0.99.55-1
- don't initialize full device lists on all probes (<ewt@redhat.com>)

* Wed Jun 19 2002 Bill Nottingham <notting@redhat.com> 0.99.54-1
- more modules.pcimap blacklisting (#66652)

* Mon Jun 17 2002 Bill Nottingham <notting@redhat.com>
- rebuild against new slang

* Wed Apr 17 2002 Bill Nottingham <notting@redhat.com>
- fix uninitialized variable (#63664)

* Mon Apr 15 2002 Bill Nottingham <notting@redhat.com>
- fix segfault in pciserial code

* Sat Apr  6 2002 Bill Nottingham <notting@redhat.com>
- set buffering for reading /proc/partitions (#61617, #56815)

* Tue Apr  2 2002 Bill Nottingham <notting@redhat.com>
- add support for USB2 (ehci-hcd)
- add device entries for *all* usb interfaces, not just the first (#52758)

* Thu Mar 21 2002 Bill Nottingham <notting@redhat.com>
- fix various ethernet device removal bugs (#61169)

* Fri Feb 22 2002 Bill Nottingham <notting@redhat.com>
- rebuild

* Thu Jan 31 2002 Bill Nottingham <notting@redhat.com>
- quick hack

* Wed Jan 30 2002 Bill Nottingham <notting@redhat.com>
- require hwdata, tweak paths accordingly
- require eepro100-diag on ia64

* Tue Jan 15 2002 Bill Nottingham <notting@redhat.com>
- rename config file to updfstab.conf

* Mon Jan 07 2002 Erik Troan <ewt@redhat.com>
- updated updfstab to use a config file

* Mon Jan  7 2002 Bill Nottingham <notting@redhat.com>
- don't print out VBE videocards when asked for DDC monitors, and vice
  versa

* Fri Jan  4 2002 Bill Nottingham <notting@redhat.com>
- fix LRMI to work with pthreads

* Thu Jan  3 2002 Bill Nottingham <notting@redhat.com>
- split vbe-probed memory into its own video device
- fix a segfault in pci.c on bad pcitable data

* Thu Oct 11 2001 Bill Nottingham <notting@redhat.com>
- go to the head of the tree. mmm, python 2.2

* Tue Sep 25 2001 Bill Nottingham <notting@redhat.com>
- dink with eepro100 eeproms

* Sat Sep  8 2001 Bill Nottingham <notting@redhat.com>
- add G550 pci id

* Thu Sep  6 2001 Bill Nottingham <notting@redhat.com>
- fix enabling of bcm5820 in boot environment

* Tue Aug 28 2001 Bill Nottingham <notting@redhat.com>
- fix po file headers (#52701)

* Fri Aug 24 2001 Bill Nottingham <notting@redhat.com>
- only refer to things in the cards DB
- we configure CLASS_OTHER now too (#51707)

* Fri Aug 24 2001 Mike A. Harris <mharris@redhat.com>
- Updated ATI video hardware PCI ID's and sync'd with XFree86's ID's

* Wed Aug 22 2001 Mike A. Harris <mharris@redhat.com>
- Fixed Radeon QD PCI ID, and a few others.
- Fixed broken pt_BR.po file so kudzu will actually build.

* Wed Aug 22 2001 Bill Nottingham <notting@redhat.com>
- move PS/2 probe to PROBE_SAFE (fixes #52040, indirectly...)

* Wed Aug 15 2001 Bill Nottingham <notting@redhat.com>
- fix checking of module aliases during device unconfiguration (#51100)

* Mon Aug 13 2001 Bill Nottingham <notting@redhat.com>
- add another megaraid variant to the pcitable
- add some configuration for bcm5820 cards (#51707)

* Fri Aug 10 2001 Bill Nottingham <notting@redhat.com>
- pcitable & translation updates (including fixing #51479)

* Mon Aug  6 2001 Bill Nottingham <notting@redhat.com>
- enumerate cardbus bridges before the rest of the PCI bus scan
  (fixes #35136, #41972, #49842, possibly others)

* Wed Aug  1 2001 Bill Nottingham <notting@redhat.com>
- don't override generic pcitable entries with subvendor/subdevice
  specific entries from modules.pcimap unless it uses a different module
  (#46454, #50604)
- don't try to configure X if they don't appear to have it installed
  (#50088)

* Thu Jul 26 2001 Bill Nottingham <notting@redhat.com>
- fix changes from yesterday

* Wed Jul 25 2001 Bill Nottingham <notting@redhat.com>
- shrink parts of libkudzu_loader.a

* Tue Jul 24 2001 Bill Nottingham <notting@redhat.com>
- put scsi.o in libkudzu_loader.a

* Mon Jul 23 2001 Bill Nottingham <notting@redhat.com>
- USB floppy probing, via scsi.c ugliness

* Mon Jul 23 2001 Florian La Roche <Florian.LaRoche@redhat.de>
- some some more gdth entries

* Thu Jul 19 2001 Bill Nottingham <notting@redhat.com>
- floppy probing!

* Mon Jul  9 2001 Bill Nottingham <notting@redhat.com>
- return fb device for VESA fb devices

* Tue May  8 2001 Bill Nottingham <notting@redhat.com>
- fix updfstab erroring out on floppies, other devices (#39623)

* Mon May  7 2001 Bill Nottingham <notting@redhat.com>
- add a couple of e1000 ids (#39391)
- use dynamic buffer size for /proc/scsi/scsi (#37936)

* Thu May  2 2001 Bill Nottingham <notting@redhat.com>
- handle CLASS_RAID like CLASS_SCSI
- put man pages in man8, not man1
- don't map all i960 stuff to megaraid; use explicit list

* Tue May 01 2001 Erik Troan <ewt@redhat.com>
- added a man page for updfstab
- updfstab added devices even when the full disk device was in fstab already

* Thu Apr 26 2001 Florian La Roche <Florian.LaRoche@redhat.de>
- add hack to not start kudzu on s390/s390x on bootup

* Mon Apr  2 2001 Preston Brown <pbrown@redhat.com>
- mark camera mount types w/a default partition.

* Fri Mar 30 2001 Bill Nottingham <notting@redhat.com>
- fix a couple of random only-on-occasion memory scribbles in pci.c

* Thu Mar 29 2001 Bill Nottingham <notting@redhat.com>
- if we're running for the first time, don't configure additional mice
  if some existing mouse is configured

* Wed Mar 28 2001 Bill Nottingham <notting@redhat.com>
- more PS/2 probe tweaks to work with yet more strange KVMs
- random pcitable tweaks (G450, etc.)

* Mon Mar 26 2001 Bill Nottingham <notting@redhat.com>
- don't segfault if they don't have a PS/2 port

* Thu Mar 22 2001 Bill Nottingham <notting@redhat.com>
- cosmetic pcitable tweaks

* Wed Mar 21 2001 Bill Nottingham <notting@redhat.com>
- don't map a particular Compaq i960 thing to megaraid (#32082)

* Tue Mar 20 2001 Erik Troan <ewt@redhat.com>
- set detached for usb devices that aren't plugged in
- don't put detached dfevices in the fstab

* Mon Mar 19 2001 Bill Nottingham <notting@redhat.com>
- only probe once in updfstab
- fix aumix lines, again (#32163)
- fix it so we don't reconfigure configured USB mice (#32236)

* Fri Mar 16 2001 Erik Troan <ewt@redhat.com>
- check for Y-E (Vaio) floppies

* Fri Mar 16 2001 Erik Troan <ewt@redhat.com>
- updfstab ignores partition tables that look invalid

* Thu Mar 14 2001 Bill Nottingham <notting@redhat.com>
- fix ps2 probe
- fix long latent bug in python module
- add new it/de/fr/es .po files

* Wed Mar 13 2001 Bill Nottingham <notting@redhat.com>
- add some documentation on the hwconf file

* Tue Mar 12 2001 Bill Nottingham <notting@redhat.com>
- new and improved PS/2 probe (<bcrl@redhat.com>)

* Mon Mar 12 2001 Bill Nottingham <notting@redhat.com>
- fix two segfaults, one in the isapnp code, one in the ddc code

* Thu Mar  8 2001 Bill Nottingham <notting@redhat.com>
- some pcitable updates
- update to updfstab (pam_console support)
- don't try and load IDE modules if we don't need to

* Wed Mar  7 2001 Bill Nottingham <notting@redhat.com>
- clean up mouse handling (#30939, #21483, #18862, others)
- ignore non-native ISAPnP stuff for now (#30805)

* Thu Mar  1 2001 Bill Nottingham <notting@redhat.com>
- fix a segfault and other weirdness in the SCSI probe (#30168)

* Wed Feb 28 2001 Bill Nottingham <notting@redhat.com>
- fix a SCSI order bug

* Tue Feb 27 2001 Preston Brown <pbrown@redhat.com>
- identify 16meg G450

* Tue Feb 27 2001 Bill Nottingham <notting@redhat.com>
- enable the ISAPnP stuff (part of #29450)
- fix a segfault

* Mon Feb 26 2001 Bill Nottingham <notting@redhat.com>
- merge in some stuff in pcitable that got lost

* Sun Feb 25 2001 Bill Nottingham <notting@redhat.com>
- don't return CLASS_NETWORK for everything in PCI_BASE_CLASS_NETWORK
  (like, say, ISDN cards) (#29308)

* Fri Feb 23 2001 Bill Nottingham <notting@redhat.com>
- only probe the PCI bus in module_upgrade (#29092, sort of)
- random cleanups, plug some memory leaks
- don't give SCSI device names to non-SCSI devices

* Fri Feb 23 2001 Preston Brown <pbrown@redhat.com>
- don't make duplicate fstab entries for devices that have the same dev entry but match different patterns.

* Thu Feb 22 2001 Bill Nottingham <notting@redhat.com>
- write aliases for multiple usb controllers

* Wed Feb 21 2001 Bill Nottingham <notting@redhat.com>
- add a new i810_audio id
- read modules.pcimap from the normal directory if we're running the BOOT
  kernel

* Tue Feb 20 2001 Bill Nottingham <notting@redhat.com>
- write lines to modules.conf to save/restore sound settings (#28504)
- fix module_upgrade for eth0 aliases

* Tue Feb 20 2001 Preston Brown <pbrown@redhat.com>
- set up agpgart on i860s.
- improvements and fixes to updfstab, including mounting IOMEGA devices in the format they prefer.

* Fri Feb 16 2001 Matt Wilson <msw@redhat.com>
- set usbDeviceList to NULL in freeUsbDevices

* Thu Feb 15 2001 Bill Nottingham <notting@redhat.com>
- fix USB multiple probe segfault

* Wed Feb 14 2001 Bill Nottingham <notting@redhat.com>
- fix updfstab up some

* Wed Feb 14 2001 Preston Brown <pbrown@redhat.com>
- final translation update

* Tue Feb 13 2001 Preston Brown <pbrown@redhat.com>
- sync pcitable w/Xconfigurator.

* Tue Feb 13 2001 Bill Nottingham <notting@redhat.com>
- fix configuration of PCI modems (#27414)

* Mon Feb 12 2001 Bill Nottingham <notting@redhat.com>
- fix python module

* Sun Feb 11 2001 Bill Nottingham <notting@redhat.com>
- expand the USB probing; return all sorts of devices, and more sane
  descriptions

* Sun Feb 11 2001 Erik Troan <ewt@redhat.com>
- added sony devices to updfstab
- look through /proc/partitions for actual partition to mount
- made symlinks optional, and only create them for /dev/cdrom

* Fri Feb  9 2001 Bill Nottingham <notting@redhat.com>
- don't bother showing a dialog for hardware we aren't going to do anything
  with
- don't configure CD-ROMs  

* Thu Feb 08 2001 Preston Brown <pbrown@redhat.com>
- update pcitable with new XFree86 stuff

* Thu Feb 08 2001 Erik Troan <ewt@redhat.com>
- added updfstab
- don't report ide devices bound to ide-scsi -- they're really scsi devices

* Wed Feb  7 2001 Matt Wilson <msw@redhat.com>
- added modules.o to LOADEROBJS in Makefile
- remove modules.o from LOADEROBJS and add stub functions in kudzu.c

* Wed Feb  7 2001 Bill Nottingham <notting@redhat.com>
- add in modules.conf upgrader
- describe the types of hardware that's added or removed in the dialog

* Mon Feb  5 2001 Bill Nottingham <notting@redhat.com>
- map disabled PCI devices to 'disabled', not to 'ignore'

* Thu Feb  1 2001 Bill Nottingham <notting@redhat.com>
- fix calling of VESA BIOS stuff (#24176)

* Wed Jan 31 2001 Bill Nottingham <notting@redhat.com>
- fix detection of disabled video boards

* Fri Jan 26 2001 Trond Eivind Glomsrød <teg@redhat.com>
- fix bug in USB detection

* Thu Jan 25 2001 Bill Nottingham <notting@redhat.com>
- fix some broken pcitable entries
- add BUS_USB to python module

* Wed Jan 24 2001 Bill Nottingham <notting@redhat.com>
- fix it so we actually pay attention to which button they push (#24858)

* Wed Jan 24 2001 Preston Brown <pbrown@redhat.com>
- final i18n update before beta

* Sun Jan 21 2001 Bill Nottingham <notting@redhat.com>
- change i18n mechanism

* Sat Jan 20 2001 Bill Nottingham <notting@redhat.com>
- add hack for 'configure/unconfigure all' in interactive mode

* Thu Jan 18 2001 Erik Troan <ewt@redhat.com>
- pcitable update for natsemi module

* Wed Jan 17 2001 Bill Nottingham <notting@redhat.com>
- pcitable updates

* Tue Jan  9 2001 Bill Nottingham <notting@redhat.com>
- don't write modules.conf aliases for cardbus network stuff

* Thu Dec 28 2000 Bill Nottingham <notting@redhat.com>
- fix continual redetection of SOCKET devices

* Fri Dec 22 2000 Bill Nottingham <notting@redhat.com>
- read modules.pcimap as well
- pcitable updates

* Tue Dec 19 2000 Bill Nottingham <notting@redhat.com>
- map yenta_socket driver for cardbus bridges (CLASS_SOCKET)

* Wed Dec 13 2000 Bill Nottingham <notting@redhat.com>
- work around a possible glibc bug
- fix up the USB device stuff

* Tue Dec 12 2000 Bill Nottingham <notting@redhat.com>
- fix segfault caused by yesterday's changes
- stub load/removeModule for the loader

* Mon Dec 11 2000 Bill Nottingham <notting@redhat.com>
- libmodules gets integrated into libkudzu
- load necessary modules before probing

* Fri Dec  8 2000 Bill Nottingham <notting@redhat.com>
- a WORM device is a CD-ROM (#19250)

* Tue Dec 05 2000 Michael Fulbright <msf@redhat.com>
- fix for IDE CDROM probing segfault

* Mon Nov 20 2000 Erik Troan <ewt@redhat.com>
- fix for scd devices > scd9

* Sat Nov 18 2000 Bill Nottingham <notting@redhat.com>
- don't use files in /tmp to determine whether to switch runlevels

* Sat Oct 21 2000 Matt Wilson <msw@redhat.com>
- install kudzu.py into /usr/lib/python1.5/site-packages, not
  /usr/lib/python1.5/
- added backwards compatibility for old python interface
- fixed crashes in python C binding

* Wed Oct 18 2000 Bill Nottingham <notting@redhat.com>
- pcitable updates
- don't 'configure' agpgart on alpha
- don't configure usb controllers if there are no modules

* Wed Oct 04 2000 Trond Eivind Glomsrød <teg@redhat.com>
- segfault fix

* Tue Oct 03 2000 Trond Eivind Glomsrød <teg@redhat.com>
- some fixes to avoid segfaulting with serial devices
  in kudzu if no PnP-description is available
- minor fix to the makefile  

* Fri Sep 29 2000 Trond Eivind Glomsrød <teg@redhat.com>
- rewritten USB support
- new python module (formerly kudzu2), giving access to
  more information 

* Wed Sep 13 2000 Trond Eivind Glomsrød <teg@redhat.com>
- include more info in the kudzu2 python module

* Fri Sep 08 2000 Trond Eivind Glomsrød <teg@redhat.com>
- new kudzu2 python module which gives access to all
  of the information available on the device in the C
  library. Hopefully will be the main module soonish.

* Wed Aug 30 2000 Bill Nottingham <notting@redhat.com>
- pcitable tweaks

* Thu Aug 24 2000 Erik Troan <ewt@redhat.com>
- updated it/es translations

* Thu Aug 24 2000 Bill Nottingham <notting@redhat.com>
- fix segfault when passed '-f <some file that doesn't exist>'

* Wed Aug 23 2000 Bill Nottingham <notting@redhat.com>
- fix some ATI Mobility mappings
- use ftw() to look for modules; it's the only sane way to handle
  2.4 kernels

* Sat Aug 19 2000 Bill Nottingham <notting@redhat.com>
- fix network device ordering

* Tue Aug 15 2000 Bill Nottingham <notting@redhat.com>
- disabled things also have IRQ 255. Neat.

* Wed Aug  9 2000 Bill Nottingham <notting@redhat.com>
- actually include translation files. Duh.

* Wed Aug  9 2000 Tim Waugh <twaugh@redhat.com>
- avoid overflowing the monitor id buffer (#15795)

* Tue Aug  8 2000 Erik Troan <ewt@redhat.com>
- look for PCMCIA IDE devices (they aren't in /proc)

* Mon Aug  7 2000 Bill Nottingham <notting@redhat.com>
- handle probing for excessive numbers of SCSI devices
- tweak IRQ 0 ignoring slightly

* Sun Aug  6 2000 Bill Nottingham <notting@redhat.com>
- ignore devices on IRQ 0

* Fri Aug  4 2000 Bill Nottingham <notting@redhat.com>
- fix subdevice sorting in pci device table (#14503)

* Fri Aug  4 2000 Florian La Roche <Florian.LaRoche@redhat.com>
- make some functions in pci.c "static"

* Wed Aug  2 2000 Bill Nottingham <notting@redhat.com>
- assorted pcitable and translation fixes

* Fri Jul 28 2000 Bill Nottingham <notting@redhat.com>
- fixes so translations get activated

* Wed Jul 26 2000 Bill Nottingham <notting@redhat.com>
- pcitable fixes (Neomagic, Matrox)

* Wed Jul 26 2000 Matt Wilson <msw@redhat.com>
- new translations for de fr it es

* Tue Jul 25 2000 Bill Nottingham <notting@redhat.com>
- pci.ids updates
- probe for memory in DDC probe
- link vbe library in directly

* Tue Jul 25 2000 Florian La Roche <Florian.LaRoche@redhat.de>
- update gdth ICP vortex entries

* Mon Jul 24 2000 Bill Nottingham <notting@redhat.com>
- turn off DDC probing in generic hardware probe
- random pcitable updates

* Tue Jul 18 2000 Michael Fulbright <msf@redhat.com>
- enable USB bus probing for loader

* Tue Jul 18 2000 Bill Nottingham <notting@redhat.com>
- pcitable updates

* Fri Jul 14 2000 Matt Wilson <msw@redhat.com>
- added USB probing to kudzu_loader library

* Fri Jul 14 2000 Bill Nottingham <notting@redhat.com>
- move initscript back

* Tue Jul 11 2000 Florian La Roche <Florian.LaRoche@redhat.de>
- add another ncr/sym controller to pcitable

* Mon Jul  3 2000 Trond Eivind Glomsrød <teg@redhat.com>
- USB mouse detection

* Mon Jul  3 2000 Bill Nottingham <notting@redhat.com>
- preliminary USB probing (from Trond)

* Tue Jun 27 2000 Bill Nottingham <notting@redhat.com>
- add /etc/sysconfig/kudzu where you can force only safe probes on boot

* Mon Jun 26 2000 Bill Nottingham <notting@redhat.com>
- configure USB controllers
- initscript path munging

* Sun Jun 18 2000 Bill Nottingham <notting@redhat.com>
- fix broken bus handling in library

* Thu Jun 15 2000 Bill Nottingham <notting@redhat.com>
- add r128 driver mappings

* Thu Jun 15 2000 Matt Wilson <msw@redhat.com>
- hacks to probe vesa and vga16 framebuffers

* Tue Jun 13 2000 Bill Nottingham <notting@redhat.com>
- DDC probing fixes

* Wed Jun  7 2000 Bill Nottingham <notting@redhat.com>
- add in monitor probing

* Mon Jun  4 2000 Bill Nottingham <notting@redhat.com>
- pcitable fixes

* Thu Jun  1 2000 Bill Nottingham <notting@redhat.com>
- modules.confiscation

* Tue May 30 2000 Erik Troan <ewt@redhat.com>
- moved kudzumodule to main kudzu package

* Wed May 10 2000 Bill Nottingham <notting@redhat.com>
- add support for PCI subvendor, subdevice IDs

* Tue Apr  4 2000 Bill Nottingham <notting@redhat.com>
- add fix for odd keyboard controllers

* Tue Mar 28 2000 Erik Troan <ewt@redhat.com>
- added kudzumodule to devel package
- added libkudzu_loader to devel

* Sat Mar  4 2000 Matt Wilson <msw@redhat.com>
- added 810 SVGA mapping

* Thu Mar  2 2000 Bill Nottingham <notting@redhat.com>
- fixes in pci device list merging

* Thu Feb 24 2000 Bill Nottingham <notting@redhat.com>
- fix aliasing and configuration of network devices
- only configure modules that are available

* Mon Feb 21 2000 Bill Nottingham <notting@redhat.com>
- fix handling of token ring devices

* Thu Feb 17 2000 Bill Nottingham <notting@redhat.com>
- yet more serial fixes

* Wed Feb 16 2000 Bill Nottingham <notting@redhat.com>
- more serial fixes; bring back DTR and RTS correctly

* Fri Feb  4 2000 Bill Nottingham <notting@redhat.com>
- don't run serial probe on serial console, fixed right

* Tue Feb  1 2000 Bill Nottingham <notting@redhat.com>
- fix previous fixes.

* Wed Jan 26 2000 Bill Nottingham <notting@redhat.com>
- fix add/remove logic somewhat

* Wed Jan 19 2000 Bill Nottingham <notting@redhat.com>
- don't run serial probe on serial console

* Fri Jan  7 2000 Bill Nottingham <notting@redhat.com>
- fix stupid bug in configuring scsi/net cards

* Mon Oct 25 1999 Bill Nottingham <notting@redhat.com>
- oops, don't try to configure 'unknown's.

* Mon Oct 11 1999 Bill Nottingham <notting@redhat.com>
- fix creation of /etc/sysconfig/soundcard...

* Wed Oct  6 1999 Bill Nottingham <notting@redhat.com>
- add inittab munging for sparc serial consoles...

* Thu Sep 30 1999 Bill Nottingham <notting@redhat.com>
- add sun keyboard probing (from jakub)
- add some bttv support

* Wed Sep 22 1999 Bill Nottingham <notting@redhat.com>
- run 'telinit 5' if needed in the initscript

* Mon Sep 20 1999 Bill Nottingham <notting@redhat.com>
- new & improved UI
- module aliasing fixes

* Thu Sep  9 1999 Bill Nottingham <notting@redhat.com>
- sanitize, homogenize, sterilize...

* Wed Sep  8 1999 Bill Nottingham <notting@redhat.com>
- get geometry for ide drives
- enumerate buses (jj@ultra.linux.cz)
