Allgemeiner Aufbau
==================

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
    		<test timeout="600" description="VM erstellen und starten">
    			<trigger type="true"></trigger>
    			<action type="fkt">AUTOTEST_VM_create</action>
    			<action type="fkt">AUTOTEST_VM_start</action>
    			<good type="ocr">|{Warte|minutes}</good>
    		</test>
    	</sequence>
    </testcase>


Begriffserklärung
=================
Die einzelnen Zeilen sind folgendermaßen aufgebaut, wobei die Begriffe *Tag*, *Attribut* und *Parameter* verwendet werden:

	`<Tag Attribut1="..." Attribut2="...">Parameter</Tag>`





Konstanten
==========

Die Konstanten und weitere Einstellungen stehen in der globalen Datei `settings.m23test` sowie der aktuellen m23test-Datei.

* TEST_SELENIUM_URL: Die URL, um auf die HTTP2SeleniumBridge zuzugreifen. z.B. `http://192.168.1.153:23080`
* TEST_VBOX_HOST: Auflösbarer Hostname oder IP des Systems, auf dem die VirtualBoxen laufen sollen. z.B. `tuxedo`
* TEST_VBOX_USER: Benutzer (muß in der Guppe *vboxusers* sein), der `vboxmanage` zum Erstellen, Starten, etc. aufruft. 
* TEST_VBOX_NETDEV: Netzwerkschnittstelle, die der echten Netzwerkkarte entspricht und zum Anlegen der Netzwerbrücke verwendet werden soll. z.B. `enp3s0f1`
* TEST_VBOX_IMAGE_DIR: Verzeichnis auf dem VirtualBox-Gastegebersystem, in dem die virtuellen Maschinen gespichert werden sollen. z.B. `/media/vms/`
* TEST_M23_BASE_URL: Komplette URL mit Benutzer und Paßwort zur m23-Weboberfläche. z.B. `https://god:m23@192.168.1.143/m23admin`
* TEST_M23_IP: Die aus `TEST_M23_BASE_URL` extrahierte IP-Adresse.
* TEST_VBOX_MAC: Beim Starten zufällig generierte MAC-Adresse mit ":" als Trenner nach jeweils zwei Zeichen. z.B. `aa:bb:cc:dd:ee:ff:00:11`
* SEL_VM_MAC: Dieselbe Zufalls-MAC, allerdings ohne den Trenner. z.B. `aabbccddeeff0011`
* TEST_TYPE: Aktuell immer "VM", da VirtualBox verwendet wird.
* VM_RAM: RAM-Größe der VM in MB.
* VM_HDSIZE: Größe der virtuellen Festplatte in MB.

Beispiel:

    <constant>
        <TEST_TYPE>VM</TEST_TYPE>
        <VM_RAM>1024</VM_RAM>
        <VM_HDSIZE>8192</VM_HDSIZE>
    </constant>


Testblöcke
==========
Ein Testblock umfaßt immer alle Teile eines Tests. `timeout` (in Sekunden) gibt an, wie lange auf den Trigger und das Abschließen durch ein good-Tag gewartet werden soll. Nach Überschreiten um mehr als zwei Minuten wird eine Warnung ausgegeben, nach mehr als 5 Minuten wird das Skript mit einem Fehler abgebrochen.  
`description` ist die Beschreibung, die in den Logdateien vermerkt wird.

Die einzelnen Teile werden folgendermaßen abgearbeitet:

1. Mit dem trigger-Tag wird der angegebene Test solange wiederholt, bis dieser erfolgreich war oder die Zeit abgelaufen ist.
2. Die einzelenen action-Tags werden in der angegbenen Reihenfolge abgearbeitet.
3. Die good/warn/bad-Tags werden immer wieder durchlaufen, bis eine Bedingung zutrifft. `bad` führt zum Abbruch, die anderen (nur) zu einem Eintrag in die Logdatei und Ausführen des nächsten Testblocks.

    <test timeout="600" description="VM erstellen und starten">
    	<trigger type="true"></trigger>
    	<action type="fkt">AUTOTEST_VM_create</action>
    	<action type="fkt">AUTOTEST_VM_start</action>
    	<good type="ocr">|{Warte|minutes}</good>
    </test>

Kommandozeile
=============
Die im `cli`-Block definierten Tags müssen in derselben Reihenfolge auf der Kommandozeile angegeben werden. Der jeweilige Tag-Name wird als Konstante gespeichert und kann in den Ersetzungen verwendet werden. `VM_NAME` wird intern für die Aufrufe von einige Funktionen z.B. `AUTOTEST_VM_keyboardWrite` oder `AUTOTEST_sshTunnelOverServer` verwendet und muß in den meisten Fällen angegebn werden.

Das Attribut `description` ist die Beschreibung des jeweiligen Tags/Kommandozeilenparameters, die ausgegeben wird, wenn nicht die korrekte Anzahl an Parametern übergeben wird.

Beispiel:

    <cli>
        <VM_NAME description="Name der VM"></VM_NAME>
        <OS_PACKAGESOURCE description="Paketquellenliste"></OS_PACKAGESOURCE>
        <OS_DESKTOP description="Desktop"></OS_DESKTOP>
    </cli>





Ersetzungen
===========

Innerhalb des Parameters können Teile ersetzt oder für Suchen verwendet werden:

* `${...}`: "..." wird durch den Wert einer vorher definierte Konstante ersetzt.
* `|{str1|str2|str3}`: str1 ... str3 sind alternative Zeichenketten, von denen beim Vergleichen nur eine übereinstimmen muß.
* `$I18N_...`: Wird nacheinander durch die Übersetzungen in allen Sprachen ersetzt und jeweils verglichen.





Selenium
========



Trigger/good/warn/bad
---------------------

### sel_hostReady (Trigger)
Wird ausgelöst, wenn HTTP2SeleniumBridge unter der `TEST_SELENIUM_URL` erreichbar ist



### sel_sourcecontains (Trigger/good/warn/bad)
Wird ausgelöst bzw. sendet eine Nachricht, wenn der Parameter im aktuellen HTML-Quelltext des Selenium-Browsers gefunden wird.

* Parameter: Zu suchender Text.



Action
------

Selenium-Aktionen benötigen (überwiegend) das Attribut `ID` oder `name` für die Identifikation des HTML-Element, auf das sie angewendet werden sollen.



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
Ersetzt den Text eines Eingabefeldes (<TEXTAREA></TEXTAREA>, <INPUT type="text">...</INPUT>).

* Parameter: Einzugebender Text.





SSH
===

Trigger/good/warn/bad
---------------------

### ssh_commandoutput
Führt über den Umweg über den m23-Server (Konstante: `TEST_M23_IP`) auf der VM (Konstante: `VM_NAME`) einen Befehl aus und überprüft, ob in der Ausgabe der gewünschte Text vorhanden ist.

* Parameter: In der Ausgabe der SSH-Abfrage vorkommender Text.
* Attribut `cmd`: Kommando, das auf dem Zielsystem ausgeführt werden soll.





VirtualBox
==========

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




Funktionen
==========

Action
------

### fkt
Führt unter CAutoTest::executePHPFunction aufgelistete Funktionen aus.

* Parameter: Name der unter CAutoTest::executePHPFunction aufgelisteten Funktion.





Sonstiges
=========

Trigger
-------

### true
Wird sofort ausgelöst.





Fehlendes
=========
In HTTP2SeleniumBridge.py sind zusätzlich folgende Kommandos implementiert, die aber nicht in CAutoTest.php verwendet werden:

* close
* deselectFrom
* quit