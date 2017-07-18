import cv2
import numpy as np
import os
import operator as op
#27 es el promedio de H en negro
#150 es el promedio de H en blanco
num = len(os.listdir("C:/Users/usuario/Desktop/git/RoBorregos-Japon/Vision/photos/"))


def mayormenor(img):
    for y in xrange(0, img.shape[0]):
        for x in xrange(0, img.shape[1]):
            if y == 0 and x == 0:
                mayor = img[y][x]
                menor = img[y][x]
            if img[y][x] < menor:
                menor = img[y][x]
            elif img[y][x] > mayor:
                mayor = img[y][x]
    print mayor
    print menor
    valores = [menor, mayor]
    return valores

def filter(img):
    new_image = np.zeros((img.shape[0], img.shape[1]), np.uint8)
    alpha = 2.2
    beta = 30
    for y in xrange(0, img.shape[0]):
        for x in xrange(0, img.shape[1]):
            pixel = img[y][x]
            value = pixel * float(alpha) + int(beta)
            if value >= 255:
                value = 255
            new_image[y][x] = value
    return new_image

def promedio():
    sum = 0
    for z in xrange(0, num):
        print z
        img = cv2.imread("photos/H/H({}).jpg".format(z), 0)
        img = img[img.shape[0]/8:img.shape[0],0:img.shape[1]]
        for y in xrange(0, img.shape[0]):
            for x in xrange(0, img.shape[1]):
                if y == 0 and x == 0:
                    menor = img[y][x]
                if img[y][x] > menor:
                    menor = img[y][x]
        sum = menor + sum
    print sum / num

for x in xrange(0, num):

    image = cv2.imread("photos/({}).jpg".format(x), 0)
    image = cv2.multiply(image, 3)
    blur = cv2.GaussianBlur(gray,(5,5),0)
    edged = cv2.Canny(blur, 20, 60)


    cv2.imshow('image', edged)
    cv2.waitKey(0)
