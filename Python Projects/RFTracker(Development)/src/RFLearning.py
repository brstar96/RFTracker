from sklearn.ensemble import RandomForestRegressor  # RFRegressor모델 임포트
from sklearn.decomposition import PCA
from sklearn.preprocessing import StandardScaler
from sklearn.model_selection import train_test_split
import pickle
import pandas as pd  # data structures for data analysis
from src import TestReader as T

RF = RandomForestRegressor(n_estimators=150, oob_score=True, random_state=0) #RF 인스턴스 생성


CSVpath = "../Datasets/CameraPosition0.csv" #Unity3D에서 만든 CSV파일의 경로
TrainData = T.LpadData()
features = pd.DataFrame(TrainData['feature'])
targets = TrainData['target']

X_train, X_test, y_train, y_test = train_test_split(features, targets, train_size=0.8, random_state=42)
RF.fit(X_train, y_train)  # RF instance에 대해 X_train이라는 데이터를 받아와 y_train의 범주로 학습시킴.

for i in range (0,500):
    print(RF.predict(X_test))
    print(" , ")
    print(y_test)





















# #-------------------------------------------------------------------------------------------------------
#     with open('RF.pkl', 'wb') as wb_pklhandler: #Pickle파일을 만들기 위한 쓰기 전용 파일핸들러 생성
#         pickle.dump(RF, wb_pklhandler) #RF 인스턴스를 RF.pkl에 Dump
#
# else:
#
#         with open('RF.pkl', 'rb') as rb_pklhandler: #Pickle파일을 만들기 위한 읽기 전용 파일핸들러 생성
#             ImportedRF = pickle.load(rb_pklhandler) #Pickle로부터 미리 학습된 모델 임포트
#
# # -----------------------------------------------------------------------------
