#모델에 테스트 이미지 넣어서 나온 세 장의 이미지(input.jpg, output.jpg, target.jpg)를 한장으로 붙이는 코드

import cv2
import numpy
import os as os

TestImagePath = "./TestImages"
TestImages = os.listdir(TestImagePath)
TestImages.sort()

red = (0, 0, 255)
green = (0, 255, 0)
blue = (255, 0, 0)
white = (255, 255, 255)
yellow = (0, 255, 255)
cyan = (255, 255, 0)
magenta = (255, 0, 255)
black = (0,0,0)
thickness = 2

# org = ((int)(img_w / 2) - 200, (int)(img_h / 2) - 100)
font = cv2.FONT_HERSHEY_SIMPLEX
fontScale = 0.5

# print(TestImages)
templist = [[[0 for col in range(3)] for row in range(768)] for rgb in range(256)]
newImage = numpy.array(templist)

for imgNo in range(0,int(len(TestImages)),3):
    imA = cv2.imread(TestImagePath + "/" + TestImages[imgNo])
    imB = cv2.imread(TestImagePath + "/" + TestImages[imgNo+1])
    imC = cv2.imread(TestImagePath + "/" + TestImages[imgNo+2])
    for h in range(0,256):
        for w in range(0,768):
            if(w<256):
                newImage[h][w]=imA[h][w]
            elif (255<w and w<512):
                newImage[h][w] = imB[h][w-256]
            else:
                newImage[h][w] = imC[h][w-512]

    cv2.putText(newImage, 'Input', (0,250), font, fontScale, black, thickness, cv2.LINE_AA)
    cv2.putText(newImage, 'Output', (256, 250), font, fontScale, black, thickness, cv2.LINE_AA)
    cv2.putText(newImage, 'Target', (512, 250), font, fontScale, black, thickness, cv2.LINE_AA)
    cv2.imwrite("result" + str(imgNo) + ".png", newImage)

    print(imgNo)