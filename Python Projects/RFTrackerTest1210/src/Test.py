from sklearn.ensemble import RandomForestRegressor  # RFRegressor모델 임포트
from sklearn.decomposition import PCA
from sklearn.preprocessing import StandardScaler
from sklearn.model_selection import train_test_split
import pickle
import pandas as pd  # data structures for data analysis
from src import CSVLoader as T
import timeit
import matplotlib as mal
import csv
import numpy as np
CSVpath = "C:/Users/admin/Desktop/RFTracker1205/Datasets/CameraPosition0.csv"  # Unity3D에서 만든 CSV 파일의 경로
param=['X_axis','Y_axis','Z_axis','alpha_rotation','beta_rotation','gamma_rotation']
evalParm = ['EX_axis','EY_axis','EZ_axis','Ealpha_rotation','Ebeta_rotation','Egamma_rotation']
PredictedTau = []


for tar_no in range(0,6):
    f = open(param[tar_no]+'.csv', 'w', encoding='utf-8', newline='')
    writer = csv.writer(f)
    TestSetList=[]
    TestSetList.append('predict')
    # list.append(',')
    TestSetList.append('target')
    writer.writerow(TestSetList)
    TrainData = T.LoadData(CSVpath)
    feature = pd.DataFrame(TrainData['feature'])
    targets =pd.DataFrame(TrainData['target'])

    T_list = [[]for a in range(0,6)]
    for i in range(0,2500):
        for k in range(0,6):
            T_list[k].append(targets[k][i])
    rF = RandomForestRegressor(n_estimators=500, oob_score=True, random_state=42,n_jobs=30)
    X_train, X_test, y_train, y_test = train_test_split(feature, T_list[tar_no], train_size=0.8, test_size=0.2,random_state=42)
    print("learning start"+str(tar_no))
    rF.fit(X_train,y_train)
    print("learning finish"+str(tar_no))
    TestSetSum =0
    for i in range(500):
        TestSetSum = TestSetSum+(abs((rF.predict(X_test)[i]-y_test[i])/100)) #X_test를 가지고 에러율 계산을 위한 summation
        TestSetList=[]
        TestSetList.append(rF.predict(X_test)[i]/100)
        TestSetList.append(y_test[i]/100)
        writer.writerow(TestSetList)
        TestSetList=[]
        print("tar_no",tar_no,"i= ",i)
    print("avg of TestSet error: ",param[tar_no],TestSetSum/500)
    f.close()
    print("file",param[tar_no]+".csv","closed")
    evalData = T.LoadEvalData("C:/Users/admin/Desktop/RFTracker1210/Datasets/DisplacementAndTouAtCameraPosition0.csv")
    evalFeature = evalData['EvalDataFeature']
    evalTargets = evalData['EvalDataTarget']

    evalSum =0
    evalTList = [[]for b in range(0,6)]
    evalFileHandler = open(evalParm[tar_no]+".csv",'w',encoding='utf-8', newline='')
    evalWriter = csv.writer(evalFileHandler)
    evalList = []
    evalList.append('predict')
    evalList.append('target')
    evalWriter.writerow(evalList)
    evalList=[]
    for r in range(0,500):
        for c  in range(0,6):
            evalTList[c].append(evalTargets[r][c])
    # print(evalTList[0])
    tmpTar = evalTList[tar_no]
    print(tmpTar)
    for k in range(0,500):
        evalList.append(rF.predict(evalFeature)[k]/100)
        evalList.append(tmpTar[k]/100)
        evalSum = evalSum+abs(rF.predict(evalFeature)[k]-tmpTar[k])/100
        evalWriter.writerow(evalList)
        evalList=[]
        print("tar_no", tar_no, "k= ", k)
    print("avg of TestSet error: ", evalParm[tar_no], evalSum / 500)
    evalFileHandler.close()
    print(evalParm[tar_no]+".csv file closed")