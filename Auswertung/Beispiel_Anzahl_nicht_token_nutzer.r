# Beispiel fuer die Berechnung der potenzielle Anzahl 
# an Nicht-Token-Nutzer 

# Lade Bibliothek zur Chartsgenerierung
library(ggplot2)

# CSV-Datei mit der Anzahl der Messwerte pro Tag
# je Tag zwei Eintraege für Token und Nicht-Tokennutzer
# wurde über ein Python-Skript erzeugt
werte.nt <- read.csv('messwerte_pro_tag.csv',header=FALSE)

# Spalten benennen
# anzahl - Anzahl der Messwerte
# tn - Indikator für Token/Nicht-Token-Anzahl
# ts - Timestamp (yyyymmdd)
# datum - Formatierte Datumsangabe
colnames(werte.nt)<-c('anzahl','tn','ts','datum')

# beachte nur die Zeilen mit der Nicht-Token-Anzahl
# und achte auf eine korrekte Sortierung nach dem Timestamp
werte.nt <- werte.nt[werte.nt$tn == 'n',]
werte.nt$datum<-reorder(werte.nt$datum, werte.nt$ts)

# Lade die Messwerte pro Tag für Nutzer 
# mit nur kurzer Sendedauer, entstammt einem SQL-Skript
# 
werte.kurz = read.csv('messwerte_kurz_token.csv', header = FALSE)

# Spalten benennen
# anzahl - gemessene Temperatur
# token - Token
# ts - Timestamp (yyyymmdd)
# datum - Formatierte Datumsangabe
colnames(werte.kurz)<-c('temp','token','ts', 'datum')

# Wir muessen die Anzahl der Messwerte pro Tag und 
# Tokennutzer noch berechnen

# Extrahiere Zeitangaben für die spätere Verwendung
daten<-unique(werte.kurz$datum)
datents<-unique(werte.kurz$ts)

# Groupiere die Daten nach dem Timestamp
groupdatum <- split(werte.kurz, f=werte.kurz$ts)

# Ermittle die Anzahl der Token und Messwerte pro Tag
token.anzahl <- sapply(groupdatum, FUN = function(a)  length(unique(a$token)))
temp.anzahl <- sapply(groupdatum, FUN = function(a)  length(a$temp))

# Erzeuge einen neuen Dataframe und achte auf die Sortierung nach dem Zeitstempel
df <- data.frame(datum=daten, ts=datents, tokenc=token.anzahl, tempc=temp.anzahl)
df$datum<-reorder(df$datum, df$ts)

# Kombiniere den neuen Dataframe mit dem Dataframe,
# der die Anzahl der Nicht-Tokenmessungen enthaelt
mdf <- merge(werte.nt, df, by="ts", all=TRUE)

# teile die Anzahl der Nicht-Token-Messwerte pro Tag durch 
# den Quotient aus Anzahl der Messwerte von Tokennutzer durch
# die Anzahl der Tokennutzer
mdf$anzahl.der.nichttokennutzer.pro.tag <- mdf$anzahl/ (mdf$tempc/mdf$tokenc)

# obige Operation klappt nur, wenn es überhaupt Nicht-Token-Messwerte gibt
# ansonsten erhalten wir NA, wir ersetzen diese Werte durch 0
mdf$anzahl.der.nichttokennutzer.pro.tag[is.na(mdf$anzahl.der.nichttokennutzer.pro.tag)]<-0

# Anzahl der Nicht-Token-Nutzer pro Tag runden, aufsummieren und ausgeben
print(sum(round(mdf$anzahl.der.nichttokennutzer.pro.tag)))

# Grafik mit der Anzahl der Nicht-Token-Nutzer pro Tag erzeugen
g <- ggplot(data=mdf, aes(x=datum.x, y=round(mdf$anzahl.der.nichttokennutzer.pro.tag))) + geom_bar(stat="identity")+scale_x_discrete(breaks = c('15.05.2016','15.06.2016','15.07.2016','15.08.2016'))

# ... und speichern
ggsave('nichttoken_anzahl_pro_tag.png', g)