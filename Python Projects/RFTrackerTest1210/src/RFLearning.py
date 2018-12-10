from sklearn.ensemble import RandomForestRegressor  # RFRegressor모델 임포트
# from sklearn.decomposition import PCA
# from sklearn.preprocessing import StandardScaler
from sklearn.model_selection import train_test_split
import pickle
import pandas as pd  # data structures for data analysis
from src import CSVLoader as T
mode = input("input mode:  ")
# ----------------학습데이터 초기화---------------------------------------------
if(mode=="train"):
    CSVpath = "../Datasets/CameraPosition0.csv" #Unity3D에서 만든 CSV파일의 경로
    TrainData = T.LoadData()
    features = pd.DataFrame(TrainData['feature'])
    targets = TrainData['target']
    list_x = list_y = list_z = list_a = list_b = list_c = []

    for i in range(0, 2500):
        list_x.append(targets[i][0])
        list_y.append(targets[i][1])
        list_z.append(targets[i][2])
        list_a.append(targets[i][3])
        list_b.append(targets[i][4])
        list_c.append(targets[i][5])

    print(list_x)

    # 보통 입력 데이터(feature)를 X로, 레이블(또는 출력)을 y로 표시.
    # train_test_split()는 list, numpy array, pandas 데이터프레임과 series 객체를 넣을 수 있음. (자동으로 무작위 표본을 선택해 split)
    X_train, X_test, y_train, y_test = train_test_split(features, targets, train_size=0.8, random_state=42)
    # -----------------------------------------------------------------------------

    # -------------------RF Estimator 학습수행-------------------------------------
    # n_estimators는 포레스트에 존재하는 트리의 개수,
    # oob_score는 보이지 않는('Unseen') 데이터로부터 R^2 Score를 추정하기 위해 OOB samples를 사용할지 여부,
    # random_state는 난수 생성 프로그램에서 사용되는 시드(초기값).
RF = [RandomForestRegressor(n_estimators=150, oob_score=True, random_state=0) for i in range(0,6)]
for i in range(0,6):
    RF[i] = RandomForestRegressor(n_estimators=150, oob_score=True, random_state=0) #RF 인스턴스 생성
    RF[i].fit(X_train, y_train)  # RF instance에 대해 X_train이라는 데이터를 받아와 y_train의 범주로 학습시킴.


#     # --------------------학습된 인스턴스 Export------------------------------------
#     with open('RF.pkl', 'wb') as wb_pklhandler: #Pickle파일을 만들기 위한 쓰기 전용 파일핸들러 생성
#         pickle.dump(RF, wb_pklhandler) #RF 인스턴스를 RF.pkl에 Dump
#
#     # print(RF.predict(test5))
#
# else:
#         # print(RF.predict(test1))
#         with open('RF.pkl', 'rb') as rb_pklhandler: #Pickle파일을 만들기 위한 읽기 전용 파일핸들러 생성
#             ImportedRF = pickle.load(rb_pklhandler) #Pickle로부터 미리 학습된 모델 임포트
# -----------------------------------------------------------------------------


