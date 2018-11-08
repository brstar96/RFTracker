import glob
import os
import numpy,cv2
import cv2


fps = 24
# tween_mini_test(190epoch)
# tween_mini_test(output)



output_list = glob.glob("D:/dictoryParser/TestImages/Fancam_modelby1024/*.jpg")
# output_list = glob.glob("D:/dictoryParser/TestImages/NFSDataset_modelby1024/*.jpg")
# output_list = glob.glob("D:\dictoryParser\DataSets/1024Testset\TestB(GT)/*.jpg")
# output_list = glob.glob("C:/Users\MGL\Desktop/1024_TestSet_50epoch/*outputs.png")
# output_list = glob.glob("C:/Users\MGL\Desktop/1024_TestSet_50epoch/*inputs.png")


aa = numpy.array(output_list)
print(output_list)
# print(type(output_list))
# cv2.imshow("aaaa",output_list[0])
videowriter =  cv2.VideoWriter("Fancam_modelby1024.mp4",-1,fps,(512,256))
print(len(output_list))
for imgname in range(len(output_list)):
   print(type(imgname))
   print(imgname)
   frame = cv2.imread(output_list[imgname])
   videowriter.write(frame)
videowriter.release()
