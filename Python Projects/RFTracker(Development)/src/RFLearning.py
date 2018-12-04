from sklearn.ensemble import RandomForestRegressor  # RFRegressor모델 임포트
from sklearn.decomposition import PCA
from sklearn.preprocessing import StandardScaler
from sklearn.model_selection import train_test_split
import pickle
import pandas as pd  # data structures for data analysis
from src import TestReader as T

test1 = [[1.518121,1.517159,1.546599,1.537342,1.534739,1.538534,1.525011,1.551522,1.524993,1.547036,1.555918,1.527577,1.543636,1.529071,1.557896,1.538295,1.515514,1.528368,1.535996,1.517369]]
test2 = [[1.511088,1.491494,1.513328,1.511999,1.525072,1.478675,1.446301,1.50733,1.516726,1.485485,1.521876,1.482964,1.476283,1.508269,1.507579,1.477101,1.514159,1.478394,1.511223,1.444688]]
test3 = [[1.424905,1.412168,1.437142,1.432806,1.439625,1.413533,1.386262,1.435324,1.430519,1.419616,1.44597,1.411548,1.413287,1.427477,1.437954,1.412659,1.425194,1.409266,1.43165,1.384431]]
test4 = [[1.48644,1.494682,1.522793,1.510978,1.504491,1.527825,1.510691,1.529218,1.494963,1.528827,1.532072,1.513317,1.528583,1.500928,1.538688,1.529732,1.483469,1.515686,1.51043,1.504957]]
test5 = [[0.9514587,0.99748,0.9529088,0.9498433,0.9668215,0.9956124,0.846694,0.9074126,0.9787045,0.8773378,0.9475369,1.009992,0.9209002,0.9386305,0.9412153,1.008119,0.9808211,1.006848,0.9674188,0.9324721]]

RF = RandomForestRegressor(n_estimators=150, oob_score=True, random_state=0) #RF 인스턴스 생성

mode = input("input mode:  ")
# ----------------학습데이터 초기화---------------------------------------------
if(mode=="train"):
    CSVpath = "../Datasets/CameraPosition0.csv" #Unity3D에서 만든 CSV파일의 경로
    TrainData = T.LpadData()
    features = pd.DataFrame(TrainData['feature'])
    targets = TrainData['target']

    # 보통 입력 데이터(feature)를 X로, 레이블(또는 출력)을 y로 표시.
    # train_test_split()는 list, numpy array, pandas 데이터프레임과 series 객체를 넣을 수 있음. (자동으로 무작위 표본을 선택해 split)
    X_train, X_test, y_train, y_test = train_test_split(features, targets, train_size=0.8, random_state=42)
    # -----------------------------------------------------------------------------

    # -------------------RF Estimator 학습수행-------------------------------------
    # n_estimators는 포레스트에 존재하는 트리의 개수,
    # oob_score는 보이지 않는('Unseen') 데이터로부터 R^2 Score를 추정하기 위해 OOB samples를 사용할지 여부,
    # random_state는 난수 생성 프로그램에서 사용되는 시드(초기값).
    RF.fit(X_train, y_train)  # RF instance에 대해 X_train이라는 데이터를 받아와 y_train의 범주로 학습시킴.
    # -----------------------------------------------------------------------------

    # --------------------학습된 인스턴스 Export------------------------------------
    with open('RF.pkl', 'wb') as wb_pklhandler: #Pickle파일을 만들기 위한 쓰기 전용 파일핸들러 생성
        pickle.dump(RF, wb_pklhandler) #RF 인스턴스를 RF.pkl에 Dump

    print(RF.predict(test5))

else:
        print(RF.predict(test1))
        with open('RF.pkl', 'rb') as rb_pklhandler: #Pickle파일을 만들기 위한 읽기 전용 파일핸들러 생성
            ImportedRF = pickle.load(rb_pklhandler) #Pickle로부터 미리 학습된 모델 임포트

# -----------------------------------------------------------------------------


