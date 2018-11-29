import numpy as np
import csv

#Input CSV로부터 data부분(x)과 target(GT)부분을 분리해 저장하는 클래스 정의
class LoadCSV:
    def _LoadCSV(path): #함수 정의
        with open(path) as f: #f라는 이름으로 파일 핸들러 생성
            data_file = csv.reader(f)
            FirstLine = next(data_file) #CSV의 첫번째 줄을 읽어옴
            n_samples = int(FirstLine[0]) #
            n_features = int(FirstLine[1])
            print("n_samples : ", n_samples )
            print("n_samples : ", n_features)
            data = np.empty((n_samples, n_features)) #쓰레기 값으로 채워진 numpy empty 배열 생성

            target = np.empty((n_samples, ))
            SecondLine = next(data_file) #CSV의 두번째 줄을 읽어옴
            feature_names = np.array(SecondLine) #CSV의 두번째 줄을 각각 feature의 이름으로 함

            for i, d in enumerate(data_file): #CSV파일의 나머지 부분에 대해 반복수행
                data[i] = np.asarray(d[:-1], dtype=np.float64) #뒤에서 두번째까지의 값을 data라고 함
                target[i] = np.asarray(d[-1], dtype=np.float64) #맨 뒷열의 값을 target(GT)으로 함

        print(feature_names) #feature names 확인
        return {'data':data, 'feature':feature_names[:-1], 'target':target}


