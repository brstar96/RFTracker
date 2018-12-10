import os
import shutil

ParentDir  = os.listdir("./DataSets/")
print(ParentDir)

for number in range(0,len(ParentDir)):
    ImgPath = "./DataSets/"+str(ParentDir[number])+"/240/"+str(ParentDir[number])
    NewFolderPath = "./DataSets/"+str(ParentDir[number])+"/240_30/"
    print("240fps 이미지 패스 ",ImgPath)
    ImgList = os.listdir(ImgPath)
    print("240fps 이미지 수량",len(os.listdir(ImgPath)))

    print("new Folder path",NewFolderPath)

    forder = os.path.exists(NewFolderPath)
    # 매 하나의 title 하위에 그에 해당하는 240_30 폴더를 생성한다.
    if not forder:
        os.mkdir(NewFolderPath,0o700)
        print(NewFolderPath, "를 만들었습니다.. ")
    else:
        # 이미 폴더가 존재할 경우 그 폴더를 지우고 다시 생성한다. ... 기존에 폴더에 존재하는 모든 파일을 지워 빈 폴더 생생
        shutil.rmtree(NewFolderPath)
        os.mkdir(NewFolderPath,0o700)
        print(NewFolderPath,"에 있던 파일은 지워 빈 디엘토리를 생성합니다. ")
    ImgList = os.listdir(ImgPath)
    for ImgNo in range(0,len(ImgList)):
        count =0
        if((ImgNo+1)%8==0 or ImgNo==0):
            # print(ImgList[ImgNo])
            shutil.copy(ImgPath+"/"+ImgList[ImgNo],NewFolderPath)
            # os.rename(ImgPath+"/"+ImgList[ImgNo],ImgPath+ImgList[ImgNo])
