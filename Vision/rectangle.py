import cv2
import numpy as np
import os

num = len(os.listdir("C:/Users/usuario/Desktop/git/RoBorregos-Japon/Vision/photos/"))

def insert(s_img):
    #insertar imagen
    black = np.zeros((300, 300), dtype = 'uint8')
    ret, l_img = cv2.threshold(black, 70, 255, cv2.THRESH_BINARY)
    x_offset=y_offset=50
    l_img[y_offset:y_offset+s_img.shape[0], x_offset:x_offset+s_img.shape[1]] = s_img
    return l_img

input_H = cv2.imread('INPUT_H.jpg', 0)
ret, thresh_H = cv2.threshold(input_H, 127, 255, 0)
contours, hierarchy = cv2.findContours(thresh_H, cv2.RETR_CCOMP, cv2.CHAIN_APPROX_SIMPLE)
sorted_contours = sorted(contours, key=cv2.contourArea, reverse=True)
template_H = contours[1]

input_S = cv2.imread('INPUT_S.jpg', 0)
ret, thresh_S = cv2.threshold(input_S, 127, 255, 0)
contours, hierarchy = cv2.findContours(thresh_S, cv2.RETR_CCOMP, cv2.CHAIN_APPROX_SIMPLE)
sorted_contours = sorted(contours, key=cv2.contourArea, reverse=True)
template_S = contours[1]

input_U = cv2.imread('INPUT_U.jpg', 0)
ret, thresh_U = cv2.threshold(input_U, 127, 255, 0)
contours, hierarchy = cv2.findContours(thresh_U, cv2.RETR_CCOMP, cv2.CHAIN_APPROX_SIMPLE)
sorted_contours = sorted(contours, key=cv2.contourArea, reverse=True)
template_U = contours[1]

input_HB = cv2.imread('INPUT_HB.jpg', 0)
ret, thresh_HB = cv2.threshold(input_HB, 127, 255, 0)
contours, hierarchy = cv2.findContours(thresh_HB, cv2.RETR_CCOMP, cv2.CHAIN_APPROX_SIMPLE)
sorted_contours = sorted(contours, key=cv2.contourArea, reverse=True)
template_HB = contours[1]

input_SB = cv2.imread('INPUT_SB.jpg', 0)
ret, thresh_SB = cv2.threshold(input_SB, 127, 255, 0)
contours, hierarchy = cv2.findContours(thresh_SB, cv2.RETR_CCOMP, cv2.CHAIN_APPROX_SIMPLE)
sorted_contours = sorted(contours, key=cv2.contourArea, reverse=True)
template_SB = contours[1]

input_UB = cv2.imread('INPUT_UB.jpg', 0)
ret, thresh_UB = cv2.threshold(input_UB, 127, 255, 0)
contours, hierarchy = cv2.findContours(thresh_UB, cv2.RETR_CCOMP, cv2.CHAIN_APPROX_SIMPLE)
sorted_contours = sorted(contours, key=cv2.contourArea, reverse=True)
template_UB = contours[1]

for z in xrange(1, num):
    #print z
    img = cv2.imread('photos/puto ({}).jpg'.format(z), 0)
    image = img[40: 240, 20: 300]
    #brillo
    bright = cv2.multiply(image, 2.5)
    #thresh otsus
    blur = cv2.GaussianBlur(bright,(3,3),0)
    ret, thresh = cv2.threshold(blur,0,255,cv2.THRESH_BINARY_INV+cv2.THRESH_OTSU)
    #contornos
    contours, hierarchy = cv2.findContours(thresh.copy(), cv2.RETR_LIST, cv2.CHAIN_APPROX_NONE)
    sorted_contours = sorted(contours, key=cv2.contourArea, reverse=True)
    x,y,w,h = cv2.boundingRect(sorted_contours[0])
    ROI = thresh[y:y+h+30,x:x+w+30]
    new_image = insert(ROI)
    orig_image = new_image
    image = np.ones(new_image.shape, dtype = 'uint8') * 255
    #cv2.imshow('Bounding Rectangle', insert(new_image))
    #cv2.waitKey(0)

    contours, hierarchy = cv2.findContours(new_image.copy(), cv2.RETR_LIST, cv2.CHAIN_APPROX_NONE)
    sorted_contours = sorted(contours, key=cv2.contourArea, reverse=True)
    #contours, hierarchy = cv2.findContours(new_image.copy(), cv2.RETR_CCOMP, cv2.CHAIN_APPROX_SIMPLE)
    #print len(contours)
    letra = sorted_contours[0]
    cv2.drawContours(image, [letra], -1, (0,0,0), 2)
    cv2.imshow('Output', image)
    cv2.waitKey(0)

    match_H = cv2.matchShapes(template_H, letra, 3, 0.0)
    match_S = cv2.matchShapes(template_S, letra, 3, 0.0)
    match_U = cv2.matchShapes(template_U, letra, 3, 0.0)
    match_HB = cv2.matchShapes(template_HB, letra, 3, 0.0)
    match_SB = cv2.matchShapes(template_SB, letra, 3, 0.0)
    match_UB = cv2.matchShapes(template_UB, letra, 3, 0.0)

    print("H:  {}".format(match_H))
    print("S:  {}".format(match_S))
    print("U:  {}".format(match_U))
    print("HB: {}".format(match_HB))
    print("SB: {}".format(match_SB))
    print("UB:  {}".format(match_UB))
    print('\n')
    print('\n')
    cv2.destroyAllWindows()
