import cv2
import numpy
import shutil as sh
import os as os

# parentPath = "./DataSets/1023Testset"
# dirList =os.listdir(parentPath)
# print("dir List",dirList)
# for dir_no in range(0,len(dirList)):
# aPath = parentPath+"/"+dirList[0]+"/TestA(blur)"
#     print(aPath)
# imgAList = os.listdir(aPath)
# print("A path",imgAList)
# bPath = parentPath+"/"+dirList[1]+"/TestB(origin)"
# imgBList = os.listdir(bPath)
    # print("B path",imgBList)

# print("\n\n-------------------dir changed name ",dirList[dir_no] ,"it has "+str(len(imgAList))+"images---------------\n\n")
imgAList = os.listdir("D:\dictoryParser\DataSets/1024Testset\TrainA(blur)")
imgBList = os.listdir("D:\dictoryParser\DataSets/1024Testset\TrainB(origin)")

baseNameA = "D:\dictoryParser\DataSets/1024Testset\TrainA(blur)/"
baseNameB = "D:\dictoryParser\DataSets/1024Testset\TrainB(origin)/"

# print(cv2.imread(baseNameA+imgAList[0]))
for img_no in range(0,len(imgAList)):
    imA = cv2.imread(baseNameA+imgAList[img_no])
    hA,wA,_ = imA.shape

    imB = cv2.imread(baseNameB+imgBList[img_no])
    hB,wB,_ = imB.shape

    templist = [[[0 for col in range(3)] for row in range(512)]for rgb in range(256)]
    # print("templist shape",imA.shape)

    newImage = numpy.array(templist)
    h,w,_ = newImage.shape

    for height in range(0,h):
        for width in range(0,w):
            if(width<256):
                newImage[height][width] = imA[height][width]
            else:
                newImage[height][width] = imB[height][width-256]

    cv2.imwrite("D:\dictoryParser/tween/"+str(img_no)+".jpg",newImage)
    # sh.move(dirList[1]+imgAList[img_no],"./tween")
    print("file created file name:   ",imgAList[img_no])