from imutils.video.pivideostream import PiVideoStream
import imutils
import time
import serial
import cv2
import numpy as np
import os

ser = serial.Serial('/dev/ttyAMA0', 9600)
ser.flushInput()
camera = PiVideoStream().start()
time.sleep(2.0)
cLee = "H"
cImprime = "N"
x = len(os.listdir("/home/pi/photos/"))

while(True):
	img = camera.read()
	#cv2.imwrite('/home/pi/photos/{}.jpg'.format(x), img)
	crop_img = img[60: 240, 20: 300]	
	gray = cv2.cvtColor(crop_img, cv2.COLOR_BGR2GRAY)
	#filtered = bright(img)
	bright = cv2.multiply(gray, 3)
	blur = cv2.GaussianBlur(bright,(5,5),0)
	edged = cv2.Canny(blur, 20, 60)
	contours, hierarchy = cv2.findContours(edged, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
	if (len(contours) > 5):
		print("LETRA A LA PVERGA PRIMO")
		for i in xrange(5):
			ser.write("L")
		time.sleep(.5)
		
		img = camera.read()		
		cv2.imwrite('/home/pi/photos/{}.jpg'.format(x), img)
		x = x + 1
	else:
		print("NARIX")
		ser.write("N")

vs.stop()
