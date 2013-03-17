Introduction
============

Follow these instructions to get the RASServer and RASClient up and running. RASClient and RASServer are scripts for managing APF (Active Port Forwarder) tunnels. The RASClient is executed on the local machine (your computer) and communicates (it gives commands and receives and shows return values/messages) with the RASServer usually run on a machine in the internet. The RASServer does the actual creation/deletion and management of the tunnels. afclients can connect against the server and the RASClient can connect against the server, too. So the afclient can forward ports to the server securely and the RASClient redirects them to local ports. The RASServer is run on a machine both (the afclient and your machine) can access. So ports from a machine can be forwarded to another machine where no direct communication (e.g. when NAT or firewall restrictions are existing) between the both machines is possible.





Connecting to create a tunnel
-----------------------------

          Connects             Connects
afclient =========> RASServer <========= RASClient (local machine)
            APF                   SSH


Forwarding of a port thru the RASServer
---------------------------------------

          Forwards port 12345             Creates local port 12345
afclient ====================> RASServer =========================> RASClient (local machine)

When you access the local port 12345, you will talk to the daemon that listens on the afclient on the same port.





What is APF?
============

From http://packages.debian.org/de/lenny/afclient:
Active Port Forwarder is a tool for secure port forwarding. It uses ssl to increase security of communication between the server and the client. It is designed for people without an external IP who want to make some services available on the Internet. The Active Port Forwarder server (afserver) is placed on the machine with a public address, and the client (afclient) is placed on the machine behind a firewall or masquerade. This makes the second machine visible to the Internet. 





Prerequisites
=============

* A server in the internet running Debian (other distributions may work, too)
* The contents of your public SSH key file (e.g. retrievable by "cat ~/.ssh/id_dsa.pub")





Configuration of the server
===========================

01. Log into your server as root:
	E.g. ssh root@myserver.somewhere.foo

02. Create a new user for RAS:
	E.g.: adduser ras

03. Switch to your newly created user:
	E.g.: su ras

04. Connect via SSH to localhost to get the .ssh directory created with the correct access rights set. Add the fingerprint with 'yes'. Quit via Ctrl+C, when it prompts for a password (the directory has been created already).
	ssh localhost

05. Paste your SSH key to ~/.ssh/authorized_keys:
	E.g.: nano ~/.ssh/authorized_keys
		* Paste
		* Press Ctrl+X
		* Save with Y+Enter

06. Try to login from your local machine (in a new terminal) to your server as the new user (this should work without a password):
	E.g. ssh ras@myserver.somewhere.foo
		* Log out: quit

07. Copy the files RASServer and RASChatAccountManager to the home directory of the new user:
	E.g.: scp RASServer RASChatAccountManager ras@myserver.somewhere.foo:~

08. Make RASServer and RASChatAccountManager executable:
	chmod +x ~/RASServer ~/RASChatAccountManager

09. Quit the new user and switch back to root:
	exit

10. Install the apf-server:
	E.g.: apt-get install apf-server

11. Add a new sudo rule to allow the new user to set iptables rules to secure the APF ports from the outside world:
	visudo
		* Add the following line at the end (adjust ras to the name of your new user):
			ras ALL=(root) NOPASSWD:/sbin/iptables
		* Save and quit the editor

12. Login from your local machine (in a new terminal) to your server as the new user:
	E.g. ssh ras@myserver.somewhere.foo
		* Run then: sudo /sbin/iptables -L
		* If there are no error messages about missing rights, all should be fine. If not check point 11. again.

13. Create (as ras) the new file ~/.RAS.config
	E.g. nano ~/.RAS.config
		* Insert the following contents:

#Config files and directories for the chat account manager
cfgChatDir='/var/www/ras/chat/lib/data'
cfgChatC="$cfgChatDir/channels.php"
cfgChatU="$cfgChatDir/users.php"
cfgChatHtpasswd="/etc/apache/.rashtpasswd"
#Path to the RASChatAccountManager
RASChatAccountManager=~/RASChatAccountManager


#Full path on the RASServer to the script
RASServer="/home/ras/RASServer"
#Path to the config file
cfg=~/.apf/afserver.conf

		* Adjust the lines you want to change
		* Log out: quit

14. Quit the server
	exit





Install AJAX Chat 0.8.3 on the server
=====================================

01. Log into your server as root:
	E.g. ssh root@myserver.somewhere.foo

02. Create the directory mentioned in the .RAS.config under $cfgChatDir (without the trailing "/lib/data")

03. Copy ajax_chat-0.8.3.tar.bz2 to this directory and one level down.
	E.g.: scp ajax_chat-0.8.3.tar.bz2 root@myserver.somewhere.foo:/var/www/ras
	
04. Extract
	* cd /var/www/ras
	* tar xfvj ajax_chat-0.8.3.tar.bz2 #This creates ajax_chat-0.8.3
	* mv ajax_chat-0.8.3/chat .
	* rmdir ajax_chat-0.8.3/

05. Change access rights
	* chmod 550 -R /var/www/ras/chat
	* chown www-data:www-data -R /var/www/ras/chat
	* chgrp ras /var/www/ras/chat/lib/ /var/www/ras/chat/lib/data/
	* chmod 770 -R /var/www/ras/chat/lib/data/ #Make config files writable by ras

06. Configure as described under http://sourceforge.net/apps/mediawiki/ajax-chat/index.php?title=Main_Page





Configuration of the client
===========================

1. Open RASClient in your preferred editor.

2. Adjust the variables host, userHost, RASDir and RASServer to your needs.

3. Save

4. Try the RASClient (assuming it is located in the current directory, if not, switch to /mdk/m23helper/RAS).
	* Run: ./RASClient list
	* It should answer:
		Config file "/home/ras/.apf/afserver.conf" created!
		Accounts:
	* If there are errors check the configuration of RASClient and RASServer.





Simple example for creating/connecting
======================================

1. (Local machine) Create a new tunnel: ./RASClient create myfirsttunnel

2. (Local machine) Get information about the tunnel: ./RASClient info myfirsttunnel
	* Get the first manageport number and its password


3. (Remote client) connect to the RASServer:
	afclient --ignorepkeys -vvvvv -p <local port number> -n <RASServer> -m <manageport number> --pass <manageport password>
	* Fill in:
		* <local port number>: The local port number of the remote client to forward
		* <RASServer>: The FQDN or ip of the RASServer
		* <manageport number>: Value from 2.
		* <manageport password>: Value from 2.
		
4. (Local machine) Connect to your RASServer and forward the port to localhost:
	./RASClient connect myfirsttunnel
	* Remember the settings for www

5. Connect to the remote service (e.g. a webserver):
	firefox <settings from 4.>