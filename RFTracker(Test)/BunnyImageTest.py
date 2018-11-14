import cv2

data  = cv2.imread("d:/62.png",0)
width, height = data.shape
for h in range(height):
    for w in range(width):
        if(data[h][w]<255):
            print(data[h][w]/255*100)
