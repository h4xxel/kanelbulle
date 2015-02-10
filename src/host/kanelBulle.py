#!/usr/bin/env python

import sys
import bluetooth
import dbus

server_mac_address = None

bus=dbus.SystemBus()

manager = dbus.Interface(bus.get_object('org.bluez', '/'), 'org.bluez.Manager')

default_adapter_path = manager.DefaultAdapter()

adapter = dbus.Interface(bus.get_object('org.bluez', default_adapter_path), 'org.bluez.Adapter')

for device_path in adapter.ListDevices():
	device=dbus.Interface(bus.get_object('org.bluez', device_path), 'org.bluez.Device')
	prop=device.GetProperties()
	#print prop['Address']+" "+prop['Name']
	if prop['Name']=='Kanelbluetooth':
		server_mac_address=prop['Address']

if server_mac_address == None:
	print "Kanelbluetooth not found"
	sys.exit(1)

#bluetooth.discover_devices(lookup_names=True) 		
#print name		
#serverMACAddress = '20:14:10:14:93:35'
channel = 1				# rfcomm is done on channel 1
s = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
s.connect((server_mac_address, channel))

while 1:
	text = raw_input() # Note change to the old (Python 2) raw_input
	if text == "quit":
		break
	s.send(text)
s.close()
