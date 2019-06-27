Vorbereitung
============

* Konsole: Kodierung auf ISO
* Aufrufen: . demoPrompt.inc




Fenstererklärung
================

0:00 - 0:46
* VM mit HTTP2SeleniumBridge
	* Menüzeilen aus dem Weg geschoben, um Platz zu sparen
	* Ubuntu 18.04
	* Gnome
	* Firefox mit Selenium
	* Autostart von HTTP2SeleniumBridge.py
	* Nimmt Anfragen per REST entgegen
	* gibt Ausgaben zurück
	* Austausch mit m23-Server, der die ganzen Tests durchführt


0:46
* Konsole
	* SSH-Verbindung zu einem m23-Server, der die einzelnen Tests durchführt, auf Ereignisse reagiert (z.B. mit dem Simulieren von Tastendrücken, dem Anlegen und Starten von virtuellen Maschinen, usw.) und letztendlich den ganzen Testablauf steuert

	* Testskript zeigen: **1demo.sh**
		* 1:10 Hier aktiviere ich den Debugmodus über Umgebungsvariable, damit man mehr sieht und m23-autoTest gesprächiger wird. m23-autoTest gibt dann Debuginformationen zurück, die ganz aufschlußreich sind.

		* Zwei Teile
			1:25 1. Installation des m23-Server vom offiziellen ISO
			1:40 Die IP-Adresse wird diese sein
			
			1:45 2. Installation eines m23-Clients vom neu installierten m23-Server aus
			1:54 Nicht wundern, das Ganze geschieht 2x mit genau denselben Parametern.
			
				* m23-Oberflächensprache: Deutsch
				* Keine Clientarchitektur angegeben, daher amd64

* VirtualBox-Fenster (kommen später)
	* VMs werden automatisch erstellt und gestartet





Eingreifen
==========

* VM-Fenster werden per Hand verschoben
* Ggf. mit Mauszeiger auf Bildschirmstellen hinweisen
* Ggf. m23-Oberfläche erklären





Serverinstallation
==================

* VM erstellen
* ISO in das virtuelle CD-Laufwerk einlegen
* VM starten
* Von ISO booten
* Dialoge per Texterkennung identifizieren
* Fragen durch simulierte Tastatureingaben beantworten
* *Hinweis: Videoaufzeichnung*
* VM ausschalten
* VM umkonfigurieren, um ISO aus dem virtuellen CD-Laufwerk zu nehmen
* VM von von Festplatte starten
* Per SSH überprüfen, ob Dienste laufen: squid, mysql, slapd





1. Clientinstallation zum Zwischenspeichern der Pakete
======================================================

* Die erste Clientinstallation wird nicht aufgezeichnet
* Wird nur durchgeführt, damit Pakete auf dem m23-Server zwischengespeichert werden
* Zweiter Durchlauf deutlich schneller, da nichts mehr aus dem Internet heruntergeladen werden muß





Clientinstallation
==================

* Sprachauswahl der m23-Oberfläche
* Vorhandenen m23-Client mit selben Namen (von vorherigem Test) **löschen**
	* Dazugehörige VM **löschen**
* LDAP-Test:
	* Versuchen, ein Benutzerkonto im LDAP anzulegen
	* Beim Hinzufügen des m23-Clients soll dies scheitern, da das Benutzerkonto bereits vorhanden ist
* O: Clientinformationen in der Oberfläche eingeben
* V: VM erstellen
* V: VM per PXE booten
* V: Hardware erkennen und Infos an m23-Server schicken
* O: Partitionieren und Formatieren
* O: Distribution + Desktop wählen
* V: VM bootet nach Abschluß der Installation von **Festplatte** neu





Testen auf Distribution und Anmeldebildschirm
=============================================

* Überprüfen der laufenden Distribution per SSH: |{Debian|Ubuntu|Mint}">cat /etc/issue</good>
* Überprüfen ob Anmeldebildschirm da ist:
	* Schlüsselworte bei Texterkennung: Anmelden|Log In|password|Log_n|...
	* Alternativ per SSH testen, ob Xorg läuft





Paketinstallation und -deinstallation von Midnight Commander
============================================================

* O: Client in Installationsliste suchen
* O: Paket "mc" suchen
* O: Paket zum Installieren markieren
* O: Paket installieren
* V: Warten auf Abschluß der Installation per SSH: dpkg --get-selections | grep -v deinstall$
* O: Client in Deinstallationsliste suchen
* O: mc-data in der Liste der installierten Pakete suchen
	* Mehrfach wiederholen, bis Paketstatus vom m23-Client an den m23-Server übertragen wurde.
* O: Paket zum Deinstallieren markieren
* V: Warten auf Abschluß der Deinstallation per SSH: dpkg --get-selections | grep -v deinstall$





LDAP-Test
=========

* V: Ist das LDAP-Benutzerkonto vorhanden: getent passwd | grep atldapuser





Beenden
=======

* VM herunterfahren