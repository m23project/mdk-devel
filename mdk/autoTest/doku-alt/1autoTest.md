% autoTest + CAutoTest
% Hauke Goos-Habermann





Installation und Konfiguration
==============================

Für autoTest wird eine Testumgebung benötigt. Deren Installation und Konfiguration ist in den folgenden Dokumenten beschrieben:

* *"autoTest-Umgebung: Installation und Konfiguration"*
* *"HTTP2SeleniumBridge: Beispiele und Installation"*





autoTest starten
================

Das Starten von autoTest geschieht über das Skript `autoTest.php`, welches auf jedem m23-Server vorhanden ist:

~~~~ {#m23testxml .bash}
/mdk/autoTest/autoTest.php <Testbeschreibungsdatei (.m23test)> <Parameter>
~~~~

Der Ablauf einer jeden Installation ist in einer Testbeschreibungsdatei beschrieben, die zusätzliche Parameter über die Kommandozeile anfordern kann.





Testbeschreibungsdateien
========================

Die Testbeschreibungsdateien mit der Endung *".m23test"* beinhalten Testblöcke, die die einzelnen Schritte (z.B. Anlegen der virtuellen Maschine, in der m23-Oberfläche einzugebende Werte bzw. anzuklickende Elemente, ...) und (erwartete) Ergebnisse zum Installieren eines m23-Clients oder -Servers enthalten. Andere Teile der Datei definieren die Parameter, die über die Kommandozeile angeben werden und die Dimensionierung der anzulegenden virtuellen Maschine.



Allgemeiner Aufbau
------------------

~~~~ {#m23testxml .xml .numberLines}
<?xml version="1.0" encoding="iso-8859-1" standalone="yes"?>
<testcase>
	<constant>
	<TEST_TYPE>VM</TEST_TYPE>
		<VM_RAM>1024</VM_RAM>
		<VM_HDSIZE>8192</VM_HDSIZE>
	</constant>
	<cli>
		<VM_NAME description="Name der VM"></VM_NAME>
		<OS_PACKAGESOURCE description="Paketquellenliste"></OS_PACKAGESOURCE>
		<OS_DESKTOP description="Desktop"></OS_DESKTOP>
	</cli>
	<sequence>
		<test timeout="180" description="Client anlegen">
			<trigger type="sel_hostReady"></trigger>
			<action type="sel_open">${TEST_M23_BASE_URL}/index.php?page=addclient%26clearSession=1</action>
			<action type="sel_typeInto" ID="ED_login">test</action>
			<action type="sel_selectFrom" ID="SEL_boottype">pxe</action>
			<action type="sel_setCheck" name="CB_getSystemtimeByNTP">0</action>
			<action type="sel_selectRadio" name="SEL_ldaptype">read</action>
			<action type="sel_clickButton" name="BUT_submit"></action>
			<good type="sel_sourcecontains">$I18N_client_added</good>
			<warn type="sel_sourcecontains">unwichtig</warn>
			<bad type="sel_sourcecontains">$I18N_addNewLoginToUCSLDAPError</bad>
		</test>
		<include>langDe.m23testinc</include>
		<test timeout="600" description="VM erstellen und starten">
			<trigger type="true"></trigger>
			<action type="fkt">AUTOTEST_VM_create</action>
			<action type="fkt">AUTOTEST_VM_start</action>
			<good type="ocr">|{Warte|minutes}</good>
		</test>
	</sequence>
</testcase>
~~~~~




Begriffserklärung
-----------------

Die einzelnen Zeilen sind folgendermaßen aufgebaut, wobei die Begriffe *Tag*, *Attribut* und *Parameter* verwendet werden:

~~~~ {#m23test-attribute .xml}
	<Tag Attribut1="..." Attribut2="...">Parameter</Tag>
~~~~




Konstanten
----------

Die Konstanten und weitere Einstellungen stehen in der globalen Datei `settings.m23test` sowie in der aktuellen m23test-Datei. `settings.m23test` wird zuerst im Heimatverzeichnis des Benutzer gesucht, der `autoTest.php` startet. Wird `settings.m23test` nicht gefunden, wird die Datei im aktuellen Verzeichnis gesucht.

Hierbei können Konstanten, durch Setzten gleichnamiger Umgebungsvariablen, beim Aufruf von `autoTest.php` überschrieben werden. Zum Beispiel folgendermaßen:

`TEST_SELENIUM_URL="http://selenium.host:23080" ./autoTest.php ...`

Intern verwendete Konstanten:

* TEST_SELENIUM_URL: Die URL, um auf die HTTP2SeleniumBridge zuzugreifen. z.B. `http://192.168.1.153:23080`
* TEST_VBOX_HOST: Auflösbarer Hostname oder IP des Systems, auf dem die VirtualBoxen laufen sollen. z.B. `tuxedo`
* TEST_VBOX_USER: Benutzer (muß in der Guppe *vboxusers* sein), der `vboxmanage` zum Erstellen, Starten, etc. aufruft. 
* TEST_VBOX_NETDEV: Netzwerkschnittstelle, die der echten Netzwerkkarte entspricht und zum Anlegen der Netzwerbrücke verwendet werden soll. z.B. `enp1s0f0`
* TEST_VBOX_IMAGE_DIR: Verzeichnis auf dem VirtualBox-Gastegebersystem, in dem die virtuellen Maschinen gespichert werden sollen. z.B. `/media/vms/`
* TEST_M23_BASE_URL: Komplette URL mit Benutzer und Paßwort zur m23-Weboberfläche. z.B. `https://god:m23@192.168.1.143/m23admin`
* TEST_M23_IP: Die aus `TEST_M23_BASE_URL` extrahierte IP-Adresse.
* TEST_VBOX_MAC: Beim Starten zufällig generierte MAC-Adresse mit ":" als Trenner nach jeweils zwei Zeichen. z.B. `aa:bb:cc:dd:ee:ff:00:11`
* SEL_VM_MAC: Dieselbe Zufalls-MAC, allerdings ohne den Trenner. z.B. `aabbccddeeff0011`
* TEST_TYPE: "VM", wenn VirtualBox verwendet wird. Soll nur die m23-Oberfläche getestet werden: "webinterface".
* VM_RAM: RAM-Größe der VM in MB.
* VM_HDSIZE: Größe der virtuellen Festplatte in MB.

In der `settings.m23test` sollten minimal folgende Konstanten gesetzt sein:

~~~~ {#m23test-VM-Parameter .xml .numberLines}
<?xml version="1.0" encoding="iso-8859-1" standalone="yes"?>
<settings>
	<constant>
		<VM_RAM>1024</VM_RAM>
		<VM_HDSIZE>8192</VM_HDSIZE>
		<TEST_VBOX_HOST>vmhost</TEST_VBOX_HOST>
		<TEST_VBOX_USER>vboxbenutzer</TEST_VBOX_USER>
		<TEST_VBOX_NETDEV>enp1s0f0</TEST_VBOX_NETDEV>
		<TEST_VBOX_IMAGE_DIR>/media/vms/</TEST_VBOX_IMAGE_DIR>
		<TEST_SELENIUM_URL>http://192.168.1.153:23080</TEST_SELENIUM_URL>
		<TEST_M23_BASE_URL>http://god:m23@192.168.1.143/m23admin</TEST_M23_BASE_URL>
	</constant>
</settings>
~~~~~



Testblöcke
----------

Ein Testblock umfaßt immer alle Teile eines Tests, die folgendermaßen abgearbeitet werden:

1. Die Bedingung des trigger-Tags wird solange wiederkehrend überprüft, bis diese zutrifft oder das Zeitlimit überschritten ist. Bei einem Überschreiten wird das Skript abgebrochen.
2. Die einzelnen action-Tags werden in der angegbenen Reihenfolge abgearbeitet, wenn die Bedingung des trigger-Tags zutraf.
3. Die good/warn/bad-Tags werden immer wieder durchlaufen, bis eine Bedingung zutrifft. `bad` führt zum Abbruch, die anderen (nur) zu einem Eintrag in die Logdatei und Ausführen des nächsten Testblocks.

`timeout` (in Sekunden) gibt an, wie lange auf den Trigger und das Abschließen durch ein good-Tag gewartet werden soll. Nach Überschreiten um mehr als zwei Minuten wird eine Warnung ausgegeben, nach mehr als 5 Minuten wird das Skript mit einem Fehler abgebrochen.

`description` ist die Beschreibung, die in den Logdateien vermerkt wird.

~~~~ {#m23test-testblock .xml}
	<test timeout="600" description="VM erstellen und starten">
		<trigger type="true"></trigger>
		<action type="fkt">AUTOTEST_VM_create</action>
		<action type="fkt">AUTOTEST_VM_start</action>
		<good type="ocr">|{Warte|minutes}</good>
	</test>
~~~~



Kommandozeilenparameter
-----------------------

Die im `cli`-Block definierten Tags müssen in derselben Reihenfolge auf der Kommandozeile angegeben werden. Der jeweilige Tag-Name wird als Konstante gespeichert und kann in den Ersetzungen verwendet werden. `VM_NAME` wird intern für die Aufrufe von einige Funktionen z.B. `AUTOTEST_VM_keyboardWrite` oder `AUTOTEST_sshTunnelOverServer` verwendet und muß in den meisten Fällen angegebn werden.

Das Attribut `description` ist die Beschreibung des jeweiligen Tags/Kommandozeilenparameters, die ausgegeben wird, wenn nicht die korrekte Anzahl an Parametern übergeben wird.

Beispiel:

~~~~ {#m23test-CLI-Parameter .xml}
	<cli>
		<VM_NAME description="Name der VM"></VM_NAME>
		<OS_PACKAGESOURCE description="Paketquellenliste"></OS_PACKAGESOURCE>
		<OS_DESKTOP description="Desktop"></OS_DESKTOP>
	</cli>
~~~~





Ersetzungen
-----------

Innerhalb des Parameters können Teile ersetzt oder für Suchen verwendet werden:

* `${...}`: "..." wird durch den Wert einer vorher definierte Konstante ersetzt.
* `|{str1|str2|str3}`: str1 ... str3 sind alternative Zeichenketten, von denen beim Vergleichen nur eine übereinstimmen muß.
* `$I18N_...`: Wird nacheinander durch die Übersetzungen in allen Sprachen ersetzt und jeweils verglichen. Hierbei muß nur eine Übersetzung übereinstimmen.
* `<include>DATEI</include>`: Fügt den Inhalt der angegebene Datei an der Stelle dynamisch ein.





Selenium-Funktionen
===================

Hier sind trigger- und action-Tags aufgelistet, die über die HTTP2SeleniumBridge Selenium-Befehle ausführen.



Trigger/good/warn/bad
---------------------

### sel_hostReady (Trigger)
Wird ausgelöst, wenn HTTP2SeleniumBridge unter der `TEST_SELENIUM_URL` erreichbar ist



### sel_sourcecontains (Trigger/good/warn/bad)
Wird ausgelöst bzw. sendet eine Nachricht, wenn der Parameter im aktuellen HTML-Quelltext des Selenium-Browsers gefunden wird.

* Parameter: Zu suchender Text.



Action
------

Selenium-Aktionen benötigen (überwiegend) das Attribut `ID` oder `name` für die Identifikation des HTML-Elementes, auf das sie angewendet werden sollen.



### sel_clickButton
Klickt auf einen Button.

* Parameter: Nichts



### sel_open
Öffnet eine URL im Browser.

* Parameter: URL z.B. `${TEST_M23_BASE_URL}/index.php?page=addclient%26clearSession=1`. Hierbei müssen einige Zeichen URL-kodiert angeben werden (z.B: '&' => '%26').



### sel_selectFrom
Wählt ein Element aus einer Drop-Down-Liste.

* Parameter: Der Wert (nicht der angezeigte Text) des auszuwählenden Elements.



### sel_selectRadio
Wählt ein Element eines Radiobuttons.

* Parameter: Der Wert (nicht der angezeigte Text) des auszuwählenden Elements.



### sel_setCheck
Setzt oder entfernt den Haken einer Checkbox.

* Parameter: 0 zum Entfernen des Hakens, 1 zum Setzen.



### sel_typeInto
Ersetzt den Text eines Eingabefeldes (`<TEXTAREA></TEXTAREA>, <INPUT type="text">...</INPUT>`).

* Parameter: Einzugebender Text.





SSH-Funktionen
==============



Trigger/good/warn/bad
---------------------

### ssh_commandoutput
Führt über den Umweg über den m23-Server (Konstante: `TEST_M23_IP`) auf der VM (Konstante: `VM_NAME`) einen Befehl aus und überprüft, ob in der Ausgabe der gewünschte Text vorhanden ist.

* Parameter: In der Ausgabe der SSH-Abfrage vorkommender Text.
* Attribut `cmd`: Kommando, das auf dem Zielsystem ausgeführt werden soll.





VirtualBox-Funktionen
=====================



Action
------

### key
Sendet eine Tastensequenz (Text) an die VM (Konstante: `VM_NAME`). Nichtdruckbare Tasten (z.B. `Enter`) werden in "°" eingeschlossen. z.B. °enter°.

* Parameter: Zu sendender Text.



Trigger/good/warn/bad
---------------------

### ocr
Erstellt einen Screenshot der laufenden VM (Konstante: `VM_NAME`) und versucht den Text mit verschiednene `gocr`-Parametern zu erkennen. Wird im erkannten Text der Parameter gefunden, so wird der Trigger ausgelöst bzw. eine Nachricht gesendet.

* Parameter: Gewünschter Text.





Funktionen zum Aufrufen anderer Funktionen
==========================================



Action
------

### fkt
Führt unter CAutoTest::executePHPFunction aufgelistete Funktionen aus.

* Parameter: Name der unter CAutoTest::executePHPFunction aufgelisteten Funktion.





Sonstige Funktionen
===================



Trigger
-------

### true
Wird sofort ausgelöst.


### wait
Löst erst nach einer gewissen Zeit aus ausgelöst.

* Parameter: Zeit in Sekunden, die bis zum Auslösen gewartet werden soll.