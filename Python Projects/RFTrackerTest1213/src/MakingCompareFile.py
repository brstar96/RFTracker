from sklearn.ensemble import RandomForestRegressor  # RFRegressor모델 임포트
from sklearn.decomposition import PCA
from sklearn.preprocessing import StandardScaler
from sklearn.model_selection import train_test_split
import pickle
import pandas as pd  # data structures for data analysis
from src import CSVLoader as T
import matplotlib as mal
import csv
import numpy as np
CSVpath = "C:/Users/admin/Desktop/RFTracker1205/Datasets/CameraPosition0.csv"  # Unity3D에서 만든 CSV 파일의 경로
TrainData = T.LoadData(CSVpath)
f = TrainData['feature']
t = TrainData['target']
feature = np.array(f)  # 형 변환
feature_f = feature.astype(float)
# print(type(feature_f[0][0]))

a,b = feature.shape
for i in range(a):
    for j in range(b):
        feature[i][j] = feature[i][j]*100

target = np.array(t)
target_f = target.astype(float)

a,b = target.shape
for i in range(a):
    for j in range(b):
        target[i][j] = target[i][j]*100


# T_list =[[0 for col in range(2500)] for row in range(6)]
# for i in range(0,6):
#     for k in range(0,2500):
#         T_list[i][k] = target[k][i]

Rf_list = [[RandomForestRegressor(n_estimators=150, oob_score=True, random_state=0)] for a in range(0,6)]
for i in range(0,1):
    X_train, X_test, y_train, y_test = train_test_split(feature_f,target_f[i], train_size=0.8, test_size=0.2,random_state=42)
    scaler = StandardScaler().fit(X_train)
    X_train_scaled = pd.DataFrame(scaler.transform(X_train))
    X_test_scaled = pd.DataFrame(scaler.transform(X_test))
    pca = PCA()  # pca는 PCA()함수를 통해 생성된 인스턴스
    pca.fit(X_train_scaled)
    Rf_list[i].fit(X_train_scaled,y_train)
print("prediict: ",Rf_list[i].predict(X_train))
print("gt",y_train)