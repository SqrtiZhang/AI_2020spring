'''
@Description: 
@Author: Sqrti
@Date: 2020-06-22 17:51:31
@LastEditTime: 2020-06-25 20:40:52
@LastEditors: Sqrti
'''

import numpy as np
import math

class SVM:
    def __init__(self,kernel, gamma, d, C, epsilon, yita, theta, r, model_path="", train_data=[], label=[], iter=3):
        self.epsilon = epsilon
        self.C = C
        self.kernel = kernel
        self.gamma = gamma
        self.d = d
        self.r = r
        self.b = 0

        if(model_path == ""):
            self.alpha = np.zeros(train_data.shape[0])
            self.train_data = train_data
            self.nn = train_data.shape[0] 
            self.dd = train_data.shape[1]
            self.label = label
        else:
            self.load_model(model_path)
            
        self.K = self.cacK()
        self.yita = yita
        self.theta = theta
        self.sv = []
        self.sv_alpha = []
        self.sv_label = []
        self.iter = iter

    def cackernel(self, x1, x2):
        if(self.kernel == 0):
            return self.linearkernel(x1, x2)
        elif(self.kernel == 1):
            return self.polynomialkernel(x1, x2)
        elif(self.kernel == 2):
            return self.rbfkernel(x1, x2)
        else:
            return self.sigmoidkernel(x1, x2)

    def cacK(self):
        K = np.zeros((self.nn, self.nn))
        for i in range(self.nn):
            for j in range(self.nn):
                K[i,j] = self.cackernel(self.train_data[i,:], self.train_data[j,:])
        return np.mat(K)

    def linearkernel(self, x1, x2):
        return np.dot(x1, x2)
    
    def polynomialkernel(self, x1, x2):
        return np.power((np.dot(x1,x2) + self.r), self.d)
    
    def rbfkernel(self, x1, x2):
        return np.exp(-self.gamma * (np.dot(x1 - x2, x1 - x2)))
    
    def sigmoidkernel(self, x1, x2):
        return np.tanh(self.yita * np.dot(x1,x2) + self.theta)

    def cac_ki(self, i):
        fxi = 0
        nonzero_index = [j for j, alpha in enumerate(self.alpha) if np.fabs(alpha) >= 1e-10]
        for j in nonzero_index:
            fxi += self.alpha[j] * self.label[j] * self.K[j,i]
        fxi += self.b
        return fxi

    def ifkkt(self, i):
        fxi = self.cac_ki(i)
        yi = self.label[i]

        if(math.fabs(self.alpha[i]) < self.epsilon) and (yi * fxi >= 1):
            return True
        elif (math.fabs(self.alpha[i] - self.C) < self.epsilon) and (yi * fxi <= 1):
            return True
        elif (self.alpha[i] > -self.epsilon) and (self.alpha[i] < (self.C + self.epsilon)) and (math.fabs(yi * fxi - 1) < self.epsilon):
            return True
        return False

    def cac_E(self, i):
        return self.cac_ki(i) - self.label[i]
    
    def find_second(self, E1, index1):
        maxnum = -1
        index2 = -1
        E2 = 0
        for i in range(self.nn):
            E2_tmp = self.cac_E(i)
            if math.fabs(E1 - E2_tmp) > maxnum:
                maxnum = math.fabs(E1 - E2_tmp)
                E2 = E2_tmp
                index2 = i
        return E2, index2

    def train(self):
        count = 0
        is_conv = False
        while(count < self.iter and not is_conv):
            count += 1
            is_conv = True

            for i in range(self.nn):
                if not self.ifkkt(i):
                    E1 = self.cac_E(i)
                    E2, j = self.find_second(E1, i)
                    alpha_old_1 = self.alpha[i]
                    alpha_old_2 = self.alpha[j]
                    label1 = self.label[i]
                    label2 = self.label[j]
                    if label1 == label2:
                        L = max(0, alpha_old_1 + alpha_old_2 - self.C)
                        H = min(self.C, alpha_old_1 + alpha_old_2)
                    else:
                        L = max(0, -alpha_old_1 +alpha_old_2)
                        H = min(self.C, self.C - alpha_old_1 + alpha_old_2)
                    if L == H:
                        continue
                    k11 = self.K[i,i]
                    k22 = self.K[j,j]
                    k21 = self.K[j,i]
                    k12 = self.K[i,j]
                    if(np.fabs(k11 + k22 - 2 * k12) <= 1e-20):
                        continue
                    alpha_new_2 = alpha_old_2 + label2 * (E1 - E2) / (k11 + k22 - 2 * k12)
                    alpha_new_2 = max (L, min(H, alpha_new_2))
                    alpha_new_1 = (alpha_old_1 * label1 + alpha_old_2 * label2 - alpha_new_2 * label2) * label1
                    

                    b1 = self.b - E1 - label1 * (alpha_new_1 - alpha_old_1) * k11 - label2 * (alpha_new_2 - alpha_old_2) * k21
                    b2 = self.b - E2 - label1 * (alpha_new_1 - alpha_old_1) * k12 - label2 * (alpha_new_2 - alpha_old_2) * k22
                    
                    if(math.fabs(alpha_new_2 - alpha_old_2) > 0.0000001) or (math.fabs(alpha_new_1 - alpha_old_1) > 0.0000001):
                        is_conv = False
                        self.alpha[i] = alpha_new_1
                        self.alpha[j] = alpha_new_2
                        self.b = b1 if (0 < alpha_new_1 and alpha_new_1 < self.C) else b2 if (0 < alpha_new_2 and alpha_new_2 < self.C) else (b1 + b2) / 2


        for i in range (self.nn):
            if self.alpha[i] > 0:
                self.sv.append(self.train_data[i,:])
                self.sv_label.append(self.label[i])
                self.sv_alpha.append(self.alpha[i])

    def predict(self, test_data):
        result = []
        for x in test_data:
            fx = 0
            for i in range(len(self.sv)):
                fx += self.sv_alpha[i] * self.sv_label[i] * self.cackernel(x, self.sv[i])
            fx += self.b
            result.append(np.sign(fx))
        return result
    
    def save_model(self, save_path):
        with open(save_path + 'support_vectors.model','wb') as f:
            np.save(f, self.b)
            np.save(f, np.array(self.sv))
        with open(save_path + 'label.model','wb') as f:
            np.save(f, np.array(self.sv_label))
        with open(save_path + 'alpha.model','wb') as f:
            np.save(f, np.array(self.sv_alpha))

    def load_model(self, load_path):
        self.b = np.load(load_path + 'support_vectors.model')
        self.sv = np.load(load_path + 'support_vectors.model')
        self.sv_label = np.load(load_path + 'label.model')
        self.sv_alpha = np.load(load_path + 'alpha.model')


