from random import *
import csv

# RandNrTranslation = []
# RandNrRotation = []
RandTau = []
NrCount = 2500 #Randomize를 수행하는 횟수
NvCount = 642 #뷰포인트 개수
csvfile = open('RandomizeTest.csv', 'w', newline='')
writer = csv.writer(csvfile)

#0~8 사이의 랜덤 정수 8개 출력
#
# for i in range(0, 3):
#     for i in range(0, 3):
#         RandNrTranslation.append(randint(0,8))
# print(RandNrTranslation)
#
# #0.0~360.0 사이의 랜덤 소수점아래 3자리 실수 8개 출력
# for j in range(0, 8):
#     RandNrRotation.append(round(uniform(0.0, 360.0), 3))
# print(RandNrRotation)

#6개 파라미터 randomize(x, y, z, a, b, c)
#for i in range(0, 2500):

for i in range(0, NvCount):
    for j in range(0, NrCount):
        for k in range(0, 3):
            RandTau.append(randint(0,8))
        for l in range(0, 3):
            RandTau.append(round(uniform(0.0, 360.0), 3))
        print(RandTau)
        writer.writerow(RandTau)
        RandTau = []

