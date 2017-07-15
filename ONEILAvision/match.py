import cv2
import numpy as np

imgS = cv2.imread('S.jpg',0)
imgH = cv2.imread('H.jpg',0)
imgU = cv2.imread('U.jpg',0)
img = cv2.imread('x.jpeg',0)

threshS = cv2.threshold(imgS, 127, 255,0)[1]
threshH = cv2.threshold(imgH, 127, 255,0)[1]
threshU = cv2.threshold(imgU, 127, 255,0)[1]
thresh = cv2.threshold(img, 90, 255,0)[1]

contoursS = cv2.findContours(threshS,2,1)[0]
contoursH = cv2.findContours(threshH,2,1)[0]
contoursU = cv2.findContours(threshU,2,1)[0]
cnt = cv2.findContours(thresh,2,1)[0][0]

cv2.drawContours(img, cnt, -1, (255,0,0))
cv2.imshow('img', img)
cv2.waitKey(0)

ret = cv2.matchShapes(cnt,contoursS,1,0.0)
print ret
ret = cv2.matchShapes(cnt,contoursH,1,0.0)
print ret
ret = cv2.matchShapes(cnt,contoursU,1,0.0)
print ret
