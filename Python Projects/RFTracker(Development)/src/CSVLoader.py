import numpy as np
import pandas as pd
import csv

#Input CSV로부터 data부분(x)과 target(GT)부분을 분리해 저장하는 클래스 정의
class LoadCSV:
    def _LoadCSV(DataPath): #함수 정의
        with open(DataPath) as f: #f라는 이름으로 파일 핸들러 생성
            #df = pd.read_csv(DataPath, converters=)




            data_file = csv.reader(f)
            FirstLine = next(data_file) #CSV의 첫번째 줄을 읽어옴

            # n_samples = str(FirstLine[0], FirstLine[1]) #2500행 26열
            n_samples = int(FirstLine[0])
            n_features = int(FirstLine[1])

            data = np.empty((n_samples,n_features-6)) #쓰레기 값으로 채워진 numpy empty 배열 생성


            target = np.empty((n_samples, 6))



            print(data.shape)
            print(target.shape)
            SecondLine = next(data_file) #CSV의 두번째 줄을 읽어옴
            feature_names = np.array(SecondLine) #CSV의 두번째 줄을 각각 feature의 이름으로 함
        #
            for i, d in enumerate(data_file): #CSV파일의 나머지 부분에 대해 반복수행
                data[i] = np.asarray(d[:-6], dtype=np.float64) #뒤에서 다섯번째까지의 값을 data라고 함
                #target[i] = np.asarray(d[-1], dtype=np.float64)  #



                return {'data':data, 'feature':feature_names[:-1], 'target':target}


