#!/bin/bash

if [ $# -ne 1 ]
then
	echo "$0 <JAR file>
	Builds temporary sign keys and a self-signet certificate and signs tha given applet afterwards.
	Example: $0 myApplet.jar"
	exit 1
fi

pass='m23m23'
bits=1024
dname='CN=m23, OU=m23, O=m23, L=m23, ST=m23, C=m2'
expires=36500
name='m23'
keyfile='/tmp/keystore'
jarFile="$1"

rm $keyfile 2> /dev/null

#Generate a new public/private key pair
keytool -genkey -keystore $keyfile -validity $expires -alias $name -storepass $pass -keypass $pass -keysize $bits -dname "$dname"

#Generate a self-signed certificate
keytool -selfcert -storepass $pass -alias $name

#Sign the JAR file
jarsigner -keystore $keyfile -storepass $pass -keypass $pass "$jarFile" $name
if [ $? -eq 0 ]
then
	echo "The JAR \"$jarFile\" was signes successfully!"
fi

rm $keyfile 2> /dev/null