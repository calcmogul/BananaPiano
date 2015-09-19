#!/usr/bin/python2

import Adafruit_BBIO.GPIO as GPIO
import Adafruit_BBIO.ADC as ADC
import Adafruit_BBIO.UART as UART
import serial
from time import sleep

# Set up DIO pins
DIOs = ["P9_27", "P9_25", "P9_23"]
for i in range(0, 2):
	GPIO.setup(DIOs[i], GPIO.OUT)
	GPIO.output(DIOs[i], GPIO.HIGH)

# Set up Analog In pins
ADC.setup()
AINs = ["P9_39", "P9_37", "P9_35"]

# Set up UART
UART.setup("UART1")
ser = serial.Serial("/dev/ttyO1", 115200)
ser.close()

# Holds analog pin values
values = ['0', '0', '0', '\n']

while (1):
	if ser.isOpen():
		for i in range(0, 3):
			pinVal = ADC.read(AINs[i])

			if pinVal > 0.55:
				values[i] = '1'
			else:
				values[i] = '0'
		ser.write(''.join(values))
	else:
		ser.open()
        sleep(.05)
ser.close()
