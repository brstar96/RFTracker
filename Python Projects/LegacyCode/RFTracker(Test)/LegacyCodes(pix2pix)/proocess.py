#폴더 안에 이미지가 100장 이하이면 패스, 100장 이상이면 1/8로 파일 개수 축소
import os
import shutil
dirList = os.listdir("./Datasets/NFSDataset-Refiend")
dirListTemp = dirList
TrainFiles_dir = './tween'
TrainFileList = os.listdir(TrainFiles_dir) #tween 경로에 존재하는 파일목록 가져오기
TrainFileList.sort() #파일 이름 순서대로 정렬
print(dirList) #디렉토리 이름을 가져온다.
print(type(dirList))

toFiltering =[]
filteringMargin = 8
for  dir_no in range(len(dirList)):
    tmpDir = "./Datasets/NFSDataset-Refiend/"+dirList[dir_no]+"/testA" #한개 폴더에 있는 모든 파일을 가져온다.
    tmpImageLen=len(os.listdir(tmpDir))# 매 파일에 있는 이미지 개수
    if tmpImageLen >100:
        name = dirList[dir_no] #이미지가 100개 이상 있는 폴더 이름
        print("dir  <<   " + name, "   >>  is need to filtering")
        for no in range(0,len(TrainFileList)):
            if not TrainFileList[no].find(name):
                toFiltering.append(TrainFileList[no])
        # move
for index in range(0,len(toFiltering)):
    if index%8!=0:
        try:
            os.remove("./tween/"+toFiltering[index])
        except:
            print("faild to remove",toFiltering[index])
            pass
        print(toFiltering[index],"has been removed")
        print("---------------------\n\n")