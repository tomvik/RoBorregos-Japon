from imutils.perspective import four_point_transform
from imutils import contours
import imutils
import cv2
import numpy as np

image = cv2.imread("104.jpg", 0)
cv2.imshow('img', image)
cv2.waitKey(0)


image = imutils.resize(image, height=500)
clahe = cv2.createCLAHE(clipLimit=2.0, tileGridSize=(20,20))
cl1 = clahe.apply(image)
blurred = cv2.GaussianBlur(cl1, (5, 5), 0)
edged = cv2.Canny(blurred, 20, 60, 255)
cv2.imshow('img', edged)
cv2.waitKey(0)

clahe = cv2.createCLAHE(clipLimit=2.0, tileGridSize=(20,20))
cl1 = clahe.apply(image)
blurred = cv2.GaussianBlur(cl1, (5, 5), 0)
thresh = cv2.threshold(blurred, 0, 255, cv2.THRESH_BINARY | cv2.THRESH_OTSU)[1]
kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (1, 5))
output = cv2.morphologyEx(thresh, cv2.MORPH_OPEN, kernel)

cv2.imshow('img', output)
cv2.waitKey(0)
