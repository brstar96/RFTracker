import os
import shutil
from random import *

randomList =[]
while len(randomList)<200:
    ran_num = randint(0,5639)
    if(not randomList.__contains__(ran_num)):
        randomList.append(ran_num)

TrainAList = os.listdir("./DataSets/1024Testset/TrainA(blur)")
TrainBList = os.listdir("./DataSets/1024Testset/TrainB(origin)")
print(TrainBList)

TestAPath = "./DataSets/1024Testset/TestA(blur)/"
TestBPath = "./DataSets/1024Testset/TestB(GT)/"

TrainAPath = "./DataSets/1024Testset/TrainA(blur)/"
TrainBPath = "./DataSets/1024Testset/TrainB(origin)/"

# print(TrainAPath+str(TrainAList[randomList[0]]))
print(len(TrainAList))

for index in range(0,len(randomList)):
    # print(randomList[index])

    shutil.move(TrainAPath+TrainAList[randomList[index]],TestAPath)
    shutil.move(TrainBPath+TrainBList[randomList[index]],TestBPath)
    # toMoveIndex = randomList[index]
    # # print("index",)
    # try:
    #     check =TrainAList[toMoveIndex]
    # except Exception:
    print("#################################",TrainAList[randomList[index]])
    # # print("check",check)
