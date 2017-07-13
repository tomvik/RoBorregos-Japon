from imutils.perspective import four_point_transform
from imutils import contours
import imutils
import cv2

image = cv2.imread("x.jpeg", 0)
cv2.imshow('img', image)
cv2.waitKey(0)
# pre-process the image by resizing it, converting it to
# graycale, blurring it, and computing an edge map
image = imutils.resize(image, height=500)
blurred = cv2.GaussianBlur(image, (5, 5), 0)
edged = cv2.Canny(blurred, 50, 200, 255)
cv2.imshow('img', edged)
cv2.waitKey(0)
thresh = cv2.threshold(image, 0, 255, cv2.THRESH_BINARY | cv2.THRESH_OTSU)[1]
kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (1, 5))
thresh = cv2.morphologyEx(thresh, cv2.MORPH_OPEN, kernel)
cv2.imshow('img', thresh)
cv2.waitKey(0)
