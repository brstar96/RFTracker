class Duck_hunting:
    ducks =0
    def __init__(self,ducks):
        self.ducks = ducks

    def hunting(self):  # self 는 this에 해당
        self.ducks -=1
    def checkDucks(self):
        print(self.ducks)


dog1  = Duck_hunting(5)
dog2  = Duck_hunting(50)
dog1.hunting()
dog1.checkDucks()
dog2.checkDucks()