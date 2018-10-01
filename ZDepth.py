import numpy as np
import cv2
import csv
import pandas as pd
# 이미 훈련집합으로 지정한 csv파일을 읽어옴
#@parm path csv파일의 위치를 설명
#@return dictionary 형태로 data feature target
#author VGLAB
def LoadMyData(path):
    with open(path) as f:
        data_file = csv.reader(f)
        FirstLine = next(data_file)
        # print("temp",temp)
        n_samples = int(FirstLine[0])
        n_features = int(FirstLine[1])
        print("n_samples",n_samples)
        print("n_features",n_features)
        data = np.empty((n_samples, n_features))
        # print("data",data)
        target = np.empty((n_samples,))
        SecondLine = next(data_file)  # names of features
        feature_names = np.array(SecondLine)

        for i, d in enumerate(data_file):

            data[i] = np.asarray(d[:-1], dtype=np.float64)
            target[i] = np.asarray(d[-1], dtype=np.float64)


    print(feature_names)
    return {'data':data,'feature':feature_names[:-1],'target':target}

#훈련집합생성 학습에 사용될csv파일 만들기
#@parm degree 훈련집합에서 사용할 데이터 회전도수
#@parm ImagePath 훈련집합에서 사용할 이미지 위치
#@parm csvPath 훈련집합에서 사용할 csv파일 위치
#author VGLAB
def CreateTrainDataSet(degree,ImagePath,csvPath):
    csvfile = open(csvPath, "w", newline='')
    writer = csv.writer(csvfile)
    headList = []
    list = []
    ImgCount = int(360/degree)
    for img_no in range(0, ImgCount):
        gray_img = cv2.imread(str(ImagePath+"/") + str(img_no) + ".png", 0)  #
        print("img path",ImagePath)
        print(gray_img)
        print(str(img_no) + "th image processing")
        width, height = gray_img.shape
        if (img_no == 0):
            writer.writerow([ImgCount, width * height])
            for k in range(0, width * height):
                headList.append("x" + str(k + 1))
            headList.append("dgree")
            writer.writerow(headList)
        for i in range(0, height):
            for j in range(0, width):
                list.append(gray_img[i, j] / 255)
        list.append(img_no * 5)
        writer.writerow(list)
        list = []
#테스트 데이터를 위한 gray scale이미지 로딩밑결과 리턴
#@parm TestDegree 테스트 이미지 회전 도수
#@parm TestImagePath 테스트 이미지가 실제로 존재하는 경로
def LoadTestDataSet (TestDegree,TestImagePath):
    df_list = []
    for img_no in range(0, int(360/TestDegree)):
        gray_img = cv2.imread(TestImagePath+"/" + str(img_no) + ".png", 0)
        list = []
        width, height = gray_img.shape
        for i in range(0, height):
            for j in range(0, width):
                list.append(gray_img[i, j] / 255)
        print("img parsing no : ", img_no)
        arr = np.array(list)
        df = pd.DataFrame(arr.reshape(1, 2500))
        df_list.append(df)
    return {"df":df,"df_list":df_list}