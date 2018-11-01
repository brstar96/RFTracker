#encoding: utf-8

import cv2
import csv

csvfile = open("ZDepth0928_5degree.scv","w",newline='')
writer = csv.writer(csvfile)
# writer.writerow(["height", "width", "gray_scale"])
headList = []
list = []
ImgCount = 72 #이미지 장수 변경시 이부분 수정
for img_no in range (0,ImgCount):

    gray_img = cv2.imread(str("TrainingImages/5degree/") + str(img_no)+".png", 0) #
    print(gray_img)
    print(str(img_no)+"th image processing")
    # img = Image.open(str("new/")+str(img_no)+".png")
    width, height = gray_img.shape
    if(img_no == 0):
        writer.writerow([ImgCount,width*height])
        for k in range(0,width*height):
            headList.append("x"+str(k+1))
        writer.writerow(headList)
    for i in range (0,height):
        for j in range (0,width):
            list.append(gray_img[i, j] / 255)
    list.append(img_no*5)
    writer.writerow(list)
    list = []
# list.append("\n")


