#encoding: utf-8
from PIL import  Image
import cv2
import csv


img = Image.open("image2.png")
width,height=img.size
print("width=",width,"height=",height)
print(type(img))
img = cv2.imread("image2.png",0) #상대경로로 이미지를 불러옴
print(type(img))
# fp = open("test.","w")
with open("test.csv","w") as csvfile:
    writer = csv.writer(csvfile)
    writer.writerow(["height", "width", "gray_scale"])
    for i in range (0,height):
        for j in range (0,width):
            str1 = "pixel["+str(i)+"]"+"["+str(j)+"]"+"gray scale: "+str(img[i,j])
            print(str1)
            writer.writerow([i,j,img[i,j]])
