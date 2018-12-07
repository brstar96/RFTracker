import numpy as np
import pandas as pd
import csv

def LoadData(csvPath):
    feature=[[ 0 for x in range(0,20)] for k in range(0,2500)]
    target=[[ 0 for x in range(0,6)] for k in range(0,2500)]

    # print(np.array(feature).shape)
    with open(csvPath) as f:  # f라는 이름으로 파일 핸들러 생성
        data_file = csv.reader(f)
        for i, d in enumerate(data_file):  # CSV파일의 나머지 부분에 대해 반복수행
            feature[i]=d[:-6]
            target[i] = d[20:]
    return {'feature':feature, "target":target}
