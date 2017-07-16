import cv2
import numpy as np
import os

num = len(os.listdir("C:/Users/usuario/Desktop/git/RoBorregos-Japon/Vision/photos/"))

for x in xrange(0, num):
    image = cv2.imread('photos/({}).jpg'.format(x), 0)
    crop_img = image[image.shape[0]/4 : image.shape[0]*3/4,  image.shape[1]/4 : image.shape[1]*3/4]
    cv2.imshow('cropped', crop_img)
    cv2.waitKey(0)
