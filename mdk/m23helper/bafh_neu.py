#Der neue BAfH-Text von http://identi.ca/api/statuses/user_timeline.json?screen_name=bastardausrede&count=200&callback=? als bafh_neu.txt
bafh_text_datei= open("bafh_neu.txt","r")
bafh_text = bafh_text_datei.read()
bafh_text_datei.close()

#Der alte BAfH-Text aus m23 (bafh_ausreden.php)
bafh_text_alt_datei = open("bafh_ausreden.php", "r") 
bafh_text_alt = bafh_text_alt_datei.read()
bafh_text_alt_datei.close()


def bafh_zerkleinerer(text):
    # zerhackt den Text von der BAfH-Seite so, dass man ihn dann in die php-Datei von m23 anfuegen kann
    a = 0
    end = 0
    bafh_list = []
    alter_bafh_text = convert_alten_bafh(bafh_text)
    while True:
        start = text.find("""{"text":""", a) + 9
        if start != 8:
            end = text.find("""","truncated""", start)
            bafh_list.append(text[start:end])
            a = end
        else:
            ausgabe = """"""
            for i in range(len(bafh_list)):
                umgewandelt = convert_string(bafh_list[i])
                if bafh_text_alt.find(umgewandelt) == -1 and ausgabe.find(umgewandelt) == -1:
                    ausgabe = ausgabe + ", " + str(i + 1 + int(finde_letzte_zahl(bafh_text_alt))) + " => '" + umgewandelt + "'"
            return ausgabe
            
def convert_string(textschnipsel):
    #\u00C4 -> &Auml; , \u00E4 -> &auml; , \u00D6 -> &Ouml; , \u00F6 -> &ouml; , \u00DC -> &Uuml; , \u00FC -> &uuml; , \u00DF -> &szlig; , ' -> \\' , \/ -> /
    ergebnis = ((((((((textschnipsel.replace('\u00c4', '&Auml;')).replace('\u00e4','&auml;')).replace('\u00d6','&Ouml;')).replace('\u00f6','&ouml;')).replace('\u00dc', '&Uuml;')).replace('\u00fc', '&uuml;')).replace('\u00df', '&szlig;')).replace("'", "\\'")).replace('\\/', '/')
    return ergebnis
    
def convert_alten_bafh(textdings):
    # wandelt den Text aus der php-Datei so um, dass er mit dem anderen Text vergleichbar ist
    ergebnis = (textdings).replace("\'", "\\'")
    return ergebnis

def finde_letzte_zahl(textstueck):
    # gibt die letzte ganze Zahl (auch mehrstellig) aus einem Text zurueck
    last = max(textstueck.rfind('0'), textstueck.rfind('1'),textstueck.rfind('2'),textstueck.rfind('3'),textstueck.rfind('4'),textstueck.rfind('5'),textstueck.rfind('6'),textstueck.rfind('7'),textstueck.rfind('8'),textstueck.rfind('9'))
    i = 1
    while True:
        if textstueck[last-i] in '0123456789':
            i = i+1
        else: 
            break
    return textstueck[last-i:last+1]
    
def daten_einfueger(altertext , neuertext, suchwort, endsequenz):
   #Erstellt einen neuen Text, der den alten Text bis vor das Stichwort, dann die Daten des neuen Textes, dann die Endsequenz enthaelt
   #
   einfuegeposition = altertext.rfind(suchwort)
   neuer_text = altertext[0:einfuegeposition] + neuertext + endsequenz
   return neuer_text
 

def hauptfunktion():
    #ueberschreibt die alte Datei
    neuer_inhalt = daten_einfueger(bafh_text_alt, bafh_zerkleinerer(bafh_text) , """);""", """ );\n?>""") 
    bafh_text_alt_datei = open("bafh_ausreden.php", "w")
    bafh_text_alt_datei.write( neuer_inhalt )
    bafh_text_alt_datei.close

#print daten_einfueger(bafh_text_alt, bafh_zerkleinerer(bafh_text) , """);""", """ );\n?>""")
#print bafh_zerkleinerer(bafh_text)
hauptfunktion()
