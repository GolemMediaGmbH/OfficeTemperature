#!/usr/bin/python

# Zur Verwendung mit Temperature-Brick
# http://www.tinkerforge.com/de/doc/Hardware/Bricklets/Temperature.html

# Sie benoetigen einen Cronjob-Eintrag der Form
# 30 * * * * <Pfad zum Script>/tf_temperature.py

import urllib2
from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_temperature import BrickletTemperature

# Benutzen Sie das Formular auf http://www.golem.de/projekte/ot/doku.php#urlgen
# um die URL zu generieren
# Erforderliche Optionen:
# - "Servernamen einfuegen" auswaehlen
# - "Temperatur einfuegen" auswaehlen
url = ""

# Tinkerforge-Parameter, Standardwerte
HOST = 'localhost'
PORT = 4223
UID = '' # UID des Temperature Bricklets eintragen

def lies_temp(host, port, uid):
	temp = None
	
	try:
		ipcon = IPConnection()
		b = BrickletTemperature(uid, ipcon)
		ipcon.connect(host, port)
	
		temp = b.get_temperature() / 100.0
		
		ipcon.disconnect()
	except:
		print("Temperaturabfrage fehlgeschlagen")
		
	return temp

def send_temp(temp):
	send_url = url % temp

	try:
		result = urllib2.urlopen(send_url);
	except urllib2.HTTPError as error:
		print error.code, error.reason
		
if __name__ == '__main__':
	
	temp = lies_temp(HOST, PORT, UID)
	
	if None != temp:
		send_temp(temp)

