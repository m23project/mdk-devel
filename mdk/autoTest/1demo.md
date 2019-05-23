Vorbereitung
============

* Konsole: Kodierung auf ISO
* Aufrufen: . demoPrompt.inc




Fenstererklärung
================

* VM mit HTTP2SeleniumBridge
	* Menüzeilen aus dem Weg geschoben, um Platz zu sparen
	* Ubuntu 18.04
	* Gnome
	* Firefox mit Selenium
	* Autostart von HTTP2SeleniumBridge.py
	* Nimmt Anfragen per REST entgegen, gibt Ausgaben zurück

* Konsole
	* SSH-Verbindung zu einem m23-Server, der den Testablauf steuert
	* Testskript zeigen
		* Zwei Teile
			1. Installation des m23-Server vom ISO
			2. Installation eines m23-Clients vom neu installierten m23-Server aus
				* Zweimal durchführen, damit Pakete bereits auf dem m23-Server liegen

* VirtualBox-Fenster (kommen später)
	* VMs werden automatisch erstellt und gestartet





Eingreifen
==========

* Fenster werden per Hand verschoben
* Ggf. Teile mit der Maus zeigen
* Ggf. m23-Obefläche erklären





Clieninstallation
=================

* Sprachauswahl der m23-Oberfläche
* ggf. vorhandenen m23-Client mit selben Namen (von vorherigem Test) löschen
* Versuchen, ein Benutzerkonto im LDAP anzulegen
	* Beim Hinzufügen des m23-Clients soll dies scheitern, da das Benutzerkonto bereits vorhanden ist
