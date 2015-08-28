#!/bin/bash

# Download all compressed baseSys archives
wget -c -r http://basesys.goos-habermann.de/

# Directory for the destination of the compressed baseSys archives
buildTar7zDir='build/m23/data+scripts/packages/baseSys/'
# Directory for the package information
buildDEBIANDir='build/DEBIAN'

mkdir deb

# Run thru the compressed baseSys archives
ls basesys.goos-habermann.de/*.7z | while read tar7z
do
	# Make sure there are empty directories
	rm -r build
	mkdir -p "$buildTar7zDir" "$buildDEBIANDir"

	# Calculate the name of the Debian package and the size of the compressed baseSys archives
	tar7zBasename=$(basename $tar7z)
	debName=$tar7zBasename"_1.0_all.deb"
	installedSize=$(find $tar7z -printf "%s")

	# Write the control file
	echo "Package: $tar7zBasename
Version: 1.0
Section: base
Priority: optional
Architecture: all
Installed-Size: $installedSize
Depends: m23
Maintainer: Hauke Goos-Habermann <hhabermann@pc-kiel.de>
Description: Installs $tar7zBasename in the m23 server.
" > "$buildDEBIANDir/control"

	# Copy the compressed baseSys archives to its destination in the build directory
	cp -v $tar7z $buildTar7zDir

	# Create the Debian package
	cd build
	dpkg-deb --build . ../deb/$debName
	cd ..
done

# Generate the package index files
cd deb
touch tempmkpackages
dpkg-scanpackages -m . tempmkpackages | grep -v "Depends: $" > Packages
gzip -c Packages > Packages.gz
bzip2 -k Packages
rm tempmkpackages

# Upload the repository to SF
rsync -avPyrL -e ssh * "hhabermann,m23@frs.sourceforge.net:/home/frs/project/m/m2/m23/baseSys/"