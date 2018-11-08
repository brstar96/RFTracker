import cv2
import glob
import os
fps = 24  # 保存视频的FPS，可以适当调整

#
# imgsDir = os.listdir("./Image")
# for dir_no in range(0,len(imgsDir)):
#     imgdir =imgsDir[dir_no]+"/testB"
#     print(os.listdir(img))
#     # print(glob.glob(imgsDir[dir_no]+"/testB/*.jpg"))

dirList = os.listdir("./Img2VidTest/")
print(type(dirList))
for no in range(len(dirList)):
    videoWriter = cv2.VideoWriter(dirList[no]+".mp4", -1, fps, (256, 256))
    imgs = glob.glob("./Image/"+dirList[no]+"/testB/*.jpg")
    for imgname in imgs:
        frame = cv2.imread(imgname)
        print(frame)
        result = videoWriter.write(frame)
    videoWriter.release()
    print(dirList[no]+"finish")