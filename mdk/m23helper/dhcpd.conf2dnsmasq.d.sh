#!/bin/bash


gateway=$(grep "option routers" /m23/dhcp/dhcpd.conf | sed 's/[^0-9\.]*//g')
network=$(grep network /etc/network/interfaces | grep -v '#' | tr -d '[:blank:]' | grep '^network' | sed 's/[^0-9\.]*//g')
netmask=$(sed 's/[{;]/\n/g' /m23/dhcp/dhcpd.conf | grep netmask | head -1 | sed 's/.*netmask //')
dns=$(sed 's/[{;]/\n/g' /m23/dhcp/dhcpd.conf | grep domain-name-servers | head -1 | sed 's/.*domain-name-servers //')
broadcast=$(sed 's/[{;]/\n/g' /m23/dhcp/dhcpd.conf | grep broadcast-address | head -1 | sed 's/.*broadcast-address //')
link="/etc/dnsmasq.d/m23"
conf="/m23/dhcp/dnsmasq-dhcpd.conf"

if [ -L $link ]
then
	echo "Link file \"$link\" exists. Exiting"
	exit 0
fi

echo "port=0
log-dhcp
enable-tftp
tftp-root=/m23/tftp
dhcp-boot=tag:pxe,pxelinux.0
dhcp-option=1,$netmask
dhcp-option=3,$gateway
dhcp-option=6,$dns
dhcp-option=28,$broadcast

" > $conf

#Get all lines from the dhcpd.conf that define clients
grep 'pxelinux.0' /m23/dhcp/dhcpd.conf | while read line
do
	echo -n "$line" | sed 's/[{;]/\n/g' | awk '
	/^ host /{
		host=$2
	}

	/^ hardware ethernet /{
		mac=$3
	}

	/^ fixed-address /{
		ip=$2
	}

	END {
		print("dhcp-host="mac","host","ip",infinite");
	}
	' >> $conf
done

echo "dhcp-range=$network,static,proxy" >> $conf

ln -s $conf $link

/etc/init.d/tftpd-hpa stop
/etc/init.d/bind9 stop
/etc/init.d/dnsmasq restart

apt-get remove isc-dhcp-server bind9 bind9-host bind9utils tftpd-hpa