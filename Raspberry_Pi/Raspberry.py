#!/usr/bin/python

# Sie benoetigen einen Cronjob-Eintrag der Form
# 30 * * * * pi <Pfad zum Script>/Raspberry.py

import re, os, urllib2
import RPi.GPIO as GPIO

# Benutzen Sie das Formular auf http://www.golem.de/projekte/ot/doku.php#urlgen
# um die URL zu generieren
# Erforderliche Optionen:
# - "Servernamen einfuegen" auswaehlen
# - "Temperatur einfuegen" auswaehlen
# - Plattform "SBC"
url = ""

# Die Sensor-ID laut /sys/bus/w1/devices
sensorid = ""
sensorpfad = "/sys/bus/w1/devices/%s/w1_slave" % sensorid

pin_led = 11
 
def lies_temp(pfad):
	temp = None
	try:
		GPIO.output(11, GPIO.LOW)
		datei = open(pfad, "r")
		zeile = datei.readline()
		if re.match(r"([0-9a-f]{2} ){9}: crc=[0-9a-f]{2} YES", zeile):
			zeile = datei.readline()
			m = re.match(r"([0-9a-f]{2} ){9}t=([+-]?[0-9]+)", zeile)
			if m:
				temp = float(m.group(2))/1000
		datei.close()
	except IOError:
		print "Konnte Sensor nicht lesen"
		GPIO.output(11, GPIO.HIGH)
	return temp
	
def send_temp(temp):
	send_url = url % temp
	try:
		result = urllib2.urlopen(send_url);
	except urllib2.HTTPError as error:
		print error.code, error.reason
		GPIO.output(11, GPIO.HIGH)

if __name__ == '__main__':
	GPIO.setwarnings(False)
	GPIO.setmode(GPIO.BOARD)
	GPIO.setup(pin_led, GPIO.OUT)
	
	temp = lies_temp(sensorpfad)
	
	if None != temp:
		send_temp(temp)
	
