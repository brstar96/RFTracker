import cv2
import matplotlib.pyplot as plt
data  = cv2.imread("D:/DepthTestImgs/195.png",0)
width, height = data.shape
x=[]
y=[]
for h in range(height):
    for w in range(width):
        if(data[h][w]<255):
            print(data[h][w])
