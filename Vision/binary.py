import cv2
import numpy as np
import os

num = len(os.listdir("F:/git/RoBorregos-Japon/Vision/photoa/")) + 1

for x in xrange(1, num):
        #PROCESS TO BINARY
        image = cv2.imread("photoa/{}.jpg".format(x), 0)
        crop_img = image[image.shape[0]/8:image.shape[0],image.shape[1]/6:image.shape[1]*5/6]
        clahe = cv2.createCLAHE(clipLimit=2.0, tileGridSize=(5,5))
        cl1 = clahe.apply(crop_img)
        blurred = cv2.GaussianBlur(cl1, (5, 5), 0)
        thresh = cv2.threshold(blurred, 0, 255, cv2.THRESH_BINARY | cv2.THRESH_OTSU)[1]
        kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (1, 5))
        output = cv2.morphologyEx(thresh, cv2.MORPH_OPEN, kernel)
        #thresh = cv2.adaptiveThreshold(output,255,cv2.ADAPTIVE_THRESH_GAUSSIAN_C, cv2.THRESH_BINARY,11,2)

        #PROCESS TO CANNY EDGES
        clahe = cv2.createCLAHE(clipLimit=2.0, tileGridSize=(20,20))
        cl1 = clahe.apply(crop_img)
        blurred = cv2.GaussianBlur(output, (5, 5), 0)
        edged = cv2.Canny(blurred, 20, 60, 255)


        res = np.hstack((cl1,crop_img))
        cv2.imshow('Binary', res)
        cv2.waitKey()
