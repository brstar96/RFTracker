import numpy as np
import pandas as pd
import random
import csv
def LoadData(csvPath):
    feature=[[ 0 for x in range(0,20)] for k in range(0,2500)]
    target=[[ 0 for x in range(0,6)] for k in range(0,2500)]

    fileHandler = open("../Datasets/CameraPosition0.csv")
    csvLines = csv.reader(fileHandler)
    datalist=[]

    for line in csvLines:
            datalist.append(line)
    FloatDataList = ChangeToFloat(datalist)
    # print("feature",FloatDataList[0][:-6])
    # print("targets",FloatDataList[0][20:])
    for no in range(0,2500):
        feature[no]=FloatDataList[no][:-6]
        target[no]=FloatDataList[no][20:]

    return {"feature":feature,"target":target}

def LoadEvalData(csvPath):
        EvalDataFeature = [[0 for x in range(0, 20)] for k in range(0, 500)]
        EvalDataTarget = [[0 for x in range(0, 6)] for k in range(0, 500)]
        fileHandler = open("../Datasets/DisplacementAndTouAtCameraPosition0.csv")
        csvLines = csv.reader(fileHandler)
        datalist = []
        for line in csvLines:
            datalist.append(line)
        FloatDataList = EvalChangeToFloat(datalist)
        for no in range(0, 500):
            EvalDataFeature[no] = FloatDataList[no][:-6]
            EvalDataTarget[no] = FloatDataList[no][20:]
        return {"EvalDataFeature":EvalDataFeature,"EvalDataTarget":EvalDataTarget}


def ChangeToFloat(list):
    List = [[0 for a in range(0,26)]for b in range(0,2500)]
    for row in range(0,2500):
        for col in range(0,26):
            List[row][col] = float(list[row][col])*100
    random.shuffle(List)
    return List

def EvalChangeToFloat(list):
    List = [[0 for a in range(0, 26)] for b in range(0, 500)]
    for row in range(0, 500):
        for col in range(0, 26):
            List[row][col] = float(list[row][col]) * 100
    return List
