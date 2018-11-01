#!/usr/bin/python
# -*- coding:utf-8 -*-
import cv2
import csv
import numpy as np
# from pandas import DataFrame as pd
import pandas as pd
csvfile = open("Parser.csv","w",newline='')
writer = csv.writer(csvfile)
# writer.writerow(["height", "width", "gray_scale"])
gray_img = cv2.imread("Test/T73.png", 0)
list =[]
mylist=[]
for i in range(0,900):
    mylist.append("x"+str(i+1))
# arr =  [[0 for i in range(0, 30)]for j in range(0, 30)]
width, height = gray_img.shape
for i in range(0, height):
    for j in range(0, width):
        list.append(gray_img[i, j] / 255)
print("list: " ,list)
arr = np.array(list)
print(type(arr))
df = pd.DataFrame(arr.reshape(1,900))
print(type(df))
print(df)
