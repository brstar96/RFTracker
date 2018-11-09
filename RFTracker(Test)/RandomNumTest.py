from random import *
import csv

# RandNrTranslation = []
# RandNrRotation = []
RandTau = []
NrCount = 2500 #Randomize를 수행하는 횟수
NvCount = 642 #뷰포인트 개수

#6개 파라미터 randomize(x, y, z, a, b, c)
for i in range(0, NvCount):
    csvfile = open("./dataSets/RandomizeTest"+str(i+1)+".csv", 'w', newline='')
    writer = csv.writer(csvfile)
    for j in range(0, NrCount):
        for k in range(0, 3):
            RandTau.append(randint(0,8))
        for l in range(0, 3):
            RandTau.append(round(uniform(0.0, 360.0), 3))
        print(RandTau)
        writer.writerow(RandTau)
        RandTau = []

