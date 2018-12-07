from sklearn.ensemble import RandomForestRegressor  # RFRegressor모델 임포트
# from sklearn.decomposition import PCA
# from sklearn.preprocessing import StandardScaler
from sklearn.model_selection import train_test_split
import pickle
import pandas as pd  # data structures for data analysis
from src import CSVLoader as T
import csv
import numpy as np
# CSVpath = "C:/Users/admin/Desktop/RFTracker1205/Datasets/CameraPosition0.csv" #Unity3D에서 만든 CSV파일의 경로
CSVpath = "C:/Users/MGL/Desktop/RFTracker1205/Datasets/CameraPosition0.csv" #Unity3D에서 만든 CSV파일의 경로

TrainData = T.LoadData(CSVpath)
features = TrainData['feature']
targets = TrainData['target']
features = [x for x in features]
targets = [x for x in targets]
print(type(targets))

T_list =[]
file = open("predicted.csv","w",newline='')
writer =csv.writer(file)
list = []
f_ytrain = []
for tau in range(0,1):
    for row in range(0,2500):
        T_list.append(float(targets[row][0]))
X_train, X_test, y_train, y_test = train_test_split(features, T_list, train_size=0.8, test_size=0.2,random_state=42)

RF = RandomForestRegressor(n_estimators=150, oob_score=True, random_state=0)

RF.fit(X_train,y_train)
print("RF has been created")
for i in range(0,500):
    print(RF.predict(X_test))