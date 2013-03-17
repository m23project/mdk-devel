for i in m23 m23-ldap m23-mdk-base m23-mdk-client m23-mdk-client-server m23-mdk m23-mdk-debs m23-mdk-doc m23-mdk-helper m23-mdk-plugin m23-mdk-screenshots m23-mdk-server m23-mdk-translator m23-mdk-update m23-phpmyadmin m23-roms m23-tftp m23-vbox
do
	./quickBuild.sh $i
done
