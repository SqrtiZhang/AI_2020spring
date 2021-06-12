'''
@Description: 
@Author: Sqrti
@Date: 2020-06-23 23:46:27
@LastEditTime: 2020-06-25 22:37:44
@LastEditors: Sqrti
'''
import numpy as np
class logisticregression:
    def __init__(self, iter, lr, lamda):
        self.iter = iter
        self.lr = lr
        self.lamda = lamda

    def cost(self, w, train_data, label):
        dot_data_w = np.dot(train_data, w)
        dot_data_w = np.array(dot_data_w, dtype=np.float)
        p = np.multiply(label, np.log(1 - 1 / (1 + np.exp(dot_data_w))))
        q = np.multiply(1-label, np.log(1 / (1 + np.exp(dot_data_w))))
        return -np.sum(p + q)/len(label)

    def regularcost(self, w, train_data, label):
        regularterm = self.lamda / (2 * len(w)) * np.power(w, 2).sum()
        return self.cost(w, train_data, label) + regularterm
    
    def increase_dim(self, data):
        return np.insert(data, 0, values=(np.zeros(data.shape[0]) + 1), axis=1)
    
    def train(self, train_data, label):
        train_data = self.increase_dim(train_data)
        n = train_data.shape[0]
        d = train_data.shape[1]
        w = np.zeros((d, 1))
        grad = np.zeros((d, 1))
        label = label.reshape((n, 1))
        labelT = label.reshape((1, n)) 

        count = 0
        while True:
            dot_data_w = np.dot(train_data, w)
            dot_data_w = np.array(dot_data_w, dtype=np.float)

            p = np.dot(labelT, (train_data / (1 + np.exp(dot_data_w))))
            m = np.multiply(np.exp(dot_data_w), 1 - label)
            q = np.dot(m.transpose(), (train_data / (1 + np.exp(dot_data_w))))
            grad = -1 / n * (p - q)
            grad = grad.transpose()
            grad = grad + self.lamda / n * w
            w = w - self.lr * grad
            count += 1
            loss = self.regularcost(w, train_data, label)
            if count % 100 == 0:
                print(count,loss)
            if(loss < 0.04):
                print(count, loss)
                break
            if(count > self.iter):
                break

        np.savetxt("w", w)

    def predict(self, data):
        w = np.loadtxt("w")
        w = w.reshape((w.shape[0],1))
        data = self.increase_dim(data)
        dot_data_w = np.dot(data, w)
        dot_data_w = np.array(dot_data_w, dtype=np.float)
        n, d = data.shape
        p = np.zeros((n, 2))
        p[:,0] = 1 / (1 + np.exp(dot_data_w)).squeeze()
        p[:,1] = 1 - p[:,0]
        #print(p)
        return np.argmax(p, 1)
    
