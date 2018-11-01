import pickle
import ZDepth as MyData
from sklearn.ensemble import RandomForestRegressor  # RFRegressor모델 임포트
from sklearn.decomposition import PCA
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler
import pickle

import numpy as np  # efficient numerical computations
import pandas as pd  # data structures for data analysis
import ZDepth as MyData
entry = { }
# entry['title'] = 'Dive into history, 2009 edition'
# entry['article_link'] = 'http://diveintomark.org/archives/2009/03/27/dive-into-history-2009-edition'
# import pickle
# with open('entry.pickle', 'wb') as f:   # f는 파일핸들러.
#     pickle.dump(entry, f)
#
#
# with open('entry.pickle', 'rb') as test:
#     entry3 = pickle.load(test)
# print(type(entry3))
#
#

TrainDegree =1
TrainImagePath = "TrainingImages/1degree"
TrainDataFile = "1degreeTraining.csv"

TrainData = MyData.LoadMyData(TrainDataFile)
features = pd.DataFrame(TrainData['data'], columns=TrainData['feature'])  # 입력은 Boston 데이터셋, 칼럼은 보스턴 데이터셋 내의 인덱스를 그대로 가져옴.
targets = TrainData["target"]  # 정답 레이블은 보스턴 데이터셋의 .target멤버 안에 있는 데이터를 사용.

TestDegree =0.5
TestImagePath = "TestImages/0_5degree"
TestData  = MyData.LoadTestDataSet(TestDegree,TestImagePath)
df  = TestData["df"]
df_list = TestData["df_list"]

# 보통 입력 데이터(feature)를 X로, 레이블(또는 출력)을 y로 표시함. 이때 레이블은 single column일수도 있고 여러 열로 구성될수도 있음
X_train, X_test, y_train, y_test = train_test_split(features, targets, train_size=0.8, random_state=42)

scaler = StandardScaler().fit(X_train)
X_train_scaled = pd.DataFrame(scaler.transform(X_train), index=X_train.index.values, columns=X_train.columns.values)
X_test_scaled = pd.DataFrame(scaler.transform(X_test), index=X_test.index.values, columns=X_test.columns.values)
df_scaled_list = []
for index in range(0, int(360/TestDegree)):
    df_scaled = pd.DataFrame(scaler.transform(df_list[index]), index=df.index.values, columns=df.columns.values)
    df_scaled_list.append(df_scaled)

with open('entry.pickle', 'rb') as test:
    entry3 = pickle.load(test)


print(type(df_scaled_list))
predicted_train = entry3.predict(df_scaled_list[3])
print(predicted_train)
print(type(predicted_train))