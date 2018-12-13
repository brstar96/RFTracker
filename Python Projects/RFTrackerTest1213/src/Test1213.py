from sklearn.ensemble import RandomForestRegressor  # RFRegressor모델 임포트
import random
from sklearn.model_selection import train_test_split
import numpy as np
import random
from random import *
import matplotlib.pyplot as plt

from sklearn.decomposition import PCA

x=[[]for k in range(1000)]
y=[]
for a in range(0,1000):
    data = random()
    for b in range(20):
        x[a].append(data)

    y.append(data*2+1)
# random.shuffle(x)
print(x)
print(y)
regressor = RandomForestRegressor(n_estimators=2500, oob_score=True, random_state=42)
X_train, X_test, y_train, y_test = train_test_split(x, y, train_size=0.8, test_size=0.2,random_state=42)

regressor.fit(X_train,y_train)
result = regressor.predict(X_test)
sum = 0

plt.plot(regressor.predict(X_test),c='magenta')
plt.plot(y_test, c='cyan')
plt.show()