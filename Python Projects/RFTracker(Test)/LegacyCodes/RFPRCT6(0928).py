##2018년 9월 27일버전 RandomForest Tracker (ver 0.1.0927)


# 필수적인 라이브러리를 몇 가지 정리. 라이브러리 설치는 Pycharm File->Setting에 들어가서 수행가능
# 데이터를 보고 간단한 연산을 수행 : pandas
# 각종 머신러닝 모델 제공 : scikit-learn
# csv파일 읽고 쓰기 제공 : cv2
# Python을 위한 OpenCV라이브러리 : opencv-python

# 라이브러리 선언부
import numpy as np  # efficient numerical computations
import pandas as pd  # data structures for data analysis
import cv2
from sklearn import datasets
from sklearn.decomposition import PCA
import os

df_list = []
# Test데이터셋을 위한 코드
for img_no in range(0, 720):
    imgPath ="../TestImages/0_5degree/" + str(img_no) + ".png"
    gray_img = cv2.imread(imgPath, 0)
    list = []
    width, height = gray_img.shape
    for i in range(0, height):
        for j in range(0, width):
            list.append(gray_img[i, j] / 255)
    print("img parsing no : ", img_no)
    arr = np.array(list)
    df = pd.DataFrame(arr.reshape(1, 900))
    df_list.append(df)
tmp = df_list[90] == df_list[91]
for i in range(0, 900):
    print(tmp[i])

# print("-----------------------dfList---------------")
# 학습 입력 설정
boston = datasets.load_boston()  # 보스턴 주택 관련 데이터셋 임포트. 범죄율, 공기오염도 등 다양한 수치들이 있음.
# 데이터 세트는 모든 데이터와 데이터에 대한 일부 메타 데이터를 보유하고 있는 사전형 객체임.
# 이 데이터들은 n_samples, n_features를 갖는 .data 멤버에 저장됨.
# 지도학습 문제의 경우 하나 이상의 response variable(응답 변수. 즉, 정답 레이블)이 .target 멤버에 저장되어 있음.

features = pd.DataFrame(boston.data, columns=boston.feature_names)  # 입력은 Boston 데이터셋, 칼럼은 보스턴 데이터셋 내의 인덱스를 그대로 가져옴.
print(type(features))
# features2 = pd.DataFrame(arr)
targets = boston.target  # 정답 레이블은 보스턴 데이터셋의 .target멤버 안에 있는 데이터를 사용.

# print(targets)

from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler

# 보통 입력 데이터(feature)를 X로, 레이블(또는 출력)을 y로 표시함. 이때 레이블은 single column일수도 있고 여러 열로 구성될수도 있음
X_train, X_test, y_train, y_test = train_test_split(features, targets, train_size=0.8, random_state=42)
# train_test_split() 함수는 또한 파이썬 list, numpy array, pandas 데이터프레임과 series 객체 등을 넣을 수 있음. 자동으로 표본을 무작위로 선택해 나눔.

# 값의 표준화(scaling)는 큰 값을 가진 속성이 최종 결과에 너무 많은 영향을 주지 않도록 하는 것.
# 각 속성에 대해 평균을 계산하고 각 속성값에서 평균을 뺀 후 표준 편차로 결과를 나눔. 스케일링을 한 후 각 속성 평균은 0이 되고, 표준 편차는 1을 갖게 된다.
scaler = StandardScaler().fit(X_train)
X_train_scaled = pd.DataFrame(scaler.transform(X_train), index=X_train.index.values, columns=X_train.columns.values)
X_test_scaled = pd.DataFrame(scaler.transform(X_test), index=X_test.index.values, columns=X_test.columns.values)
df_scaled_list = []
for index in range(0, 720):
    df_scaled = pd.DataFrame(scaler.transform(df_list[index]), index=df.index.values, columns=df.columns.values)
    df_scaled_list.append(df_scaled)

print(type(df_scaled_list))
# 높은 데이터 차원은 좋은 결과를 주지 못하기 때문에('차원의 저주') PCA 등의 방법으로 차원을 낮춰 주어야 한다.
pca = PCA()  # pca는 PCA()함수를 통해 생성된 인스턴스
pca.fit(X_train)  # X_train을 PCA()함수로 차원을 낮춰준 다음 pca에 적합화(fit)하여 저장
cpts = pd.DataFrame(pca.transform(X_train))
x_axis = np.arange(1, pca.n_components_ + 1)
pca_scaled = PCA()
pca_scaled.fit(X_train_scaled)
cpts_scaled = pd.DataFrame(pca.transform(X_train_scaled))
from sklearn.ensemble import RandomForestRegressor  # RFRegressor모델 임포트

rf = RandomForestRegressor(n_estimators=500, oob_score=True, random_state=0)
# n_estimators는 포레스트에 존재하는 트리의 개수,
# oob_score는 보이지 않는('Unseen') 데이터로부터 R^2 Score를 추정하기 위해 OOB samples를 사용할지 여부,
# random_state는 난수 생성 프로그램에서 사용되는 시드(초기값).
# rf는 저자가 정의한 estimator의 instance로, RandomForestRegressor 함수로 이 인스턴스를 학습시킬 것임.
rf.fit(X_train, y_train)  # rf estimator instance에 대해 X_train이라는 데이터를 받고 y_train의 범주로 학습시킴.

# 학습데이터 출력
# print(X_train)
# print(y_train)
# print(X_test)
# print(y_test)

# 여기까지 cmd에서 돌리면 아래와 같이 RF가 형성되는것을 확인가능
# RandomForestRegressor(bootstrap=True, criterion='mse', max_depth=None,
#            max_features='auto', max_leaf_nodes=None,
#            min_impurity_decrease=0.0, min_impurity_split=None,
#            min_samples_leaf=1, min_samples_split=2,
#            min_weight_fraction_leaf=0.0, n_estimators=500, n_jobs=1,
#            oob_score=True, random_state=0, verbose=0, warm_start=False)


from sklearn.metrics import r2_score  # R2 score(Root mean squared err?) 연산을 위한 선언
# RMSE(Root Mean Squared Error)는 예측값과 실제값을 뺀 후 제곱시킨 값들을 다 더한 다음 n으로 평균내고 루트를 씌운것.
from scipy.stats import spearmanr, pearsonr

# predict메소드의 인수는 학습에 사용했던 원래 X데이터와 동일한 형태로 들어와야 한다.
predicted_train = rf.predict(X_train)
# predicted_test = rf.predict(X_test) #모든 테스트셋에 대해 테스트 수행
# predicted_test = rf.predict(df_scaled) #모든 테스트셋에 대해 테스트 수행
for index in range(0, 720):
    prediction = rf.predict(df_scaled_list[index])
    print("GT",index*0.5,"prediction : ", prediction, "도")