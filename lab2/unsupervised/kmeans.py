'''
@Description: 
@Author: Sqrti
@Date: 2020-06-25 23:56:09
@LastEditTime: 2020-06-26 13:14:50
@LastEditors: Sqrti
'''
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

def cac_distance(x, y):
    return np.sqrt(np.dot(x - y, x - y))

def cac_si(cluster, mu):
    k = len(cluster)
    sum_si = 0
    all_num = 0
    for i, c in enumerate(cluster):
        distance = np.zeros((c.shape[0], c.shape[0]))
        for m in range(c.shape[0]):
            for n in range(m + 1, c.shape[0]):
                distance[m, n] = cac_distance(c[m], c[n])
                distance[n, m] = distance[m, n]
        ai = np.sum(distance, axis=1) / np.float(c.shape[0])
        '''distance = np.zeros((c.shape[0], len(cluster) - 1))
        for m in range(c.shape[0]):
            for n in range(len(cluster) - 1):
                distance[m, n] = cac_distance(c[m], mu[(i + n + 1) % k])
        nearest_c = np.argmax(distance, axis=1)
        nearest_c = (nearest_c + i + 1) % k'''
        distance = np.zeros((c.shape[0], len(cluster) - 1))
        for m in range(c.shape[0]):
            for n in range(len(cluster) - 1):
                for y in cluster[(i + n + 1) % k]:
                    distance[m, n] += cac_distance(y, c[m])
                distance [m, n] = distance [m, n] / cluster[(i + n + 1) % k].shape[0]
        bi = np.min(distance, axis=1)
        '''    
        for m in range(c.shape[0]):
            for y in cluster[nearest_c[m]]:
                bi[m] += cac_distance(y, c[m])
            bi[m] = bi[m] / cluster[nearest_c[m]].shape[0]
        '''
        si = (bi - ai) / np.maximum(bi, ai)
        sum_si += np.sum(si)
        all_num += c.shape[0]
    return sum_si / all_num





def KMeans(data, k, iter = 150):
    mu_index = np.random.randint(data.shape[0], size=(k))
    mu = data[mu_index]
    count = 0 
    is_change = True
    cluster = [[] for i in range(k)]
    cluster_index = [[] for i in range(k)]
    while (count < iter and is_change):
        is_change = False
        cluster = [[] for i in range(k)]
        cluster_index = [[] for i in range(k)]
        distance = np.zeros((data.shape[0], k))
        for i in range(data.shape[0]):
            for j in range(k):
                distance[i, j] = cac_distance(data[i], mu[j])
        nearest = np.argmin(distance, axis=1)

        for i in range(data.shape[0]):
            cluster[nearest[i]].append(data[i,:])
            cluster_index[nearest[i]].append(i)

        for i in range(k):
            cluster[i] = np.array(cluster[i])
            cluster_n = cluster[i].shape[0]
            cluster_sum = np.sum(cluster[i], axis=0)
            mu_new = 1.0 /cluster_n * cluster_sum
            if(np.sum(np.fabs(mu_new - mu[i])) > 1e-6):
                is_change = True
                mu[i] = mu_new
        count += 1
    print(count)
    for i in range(k):
        print(cluster[i].shape[0])
    #save_cluster(cluster)
    return cluster, cac_si(cluster, mu), cluster_index

def save_cluster(cluster, path=""):
    for i, c in enumerate(cluster):
        df = pd.DataFrame(c)
        df.to_csv("cluster_%i.csv"%i, header= None, index=None)

def cac_ri(cluster_index, label):
    result_c = np.zeros(label.shape[0])
    for i, c in enumerate(cluster_index):
        for index in c:
            result_c[index] = i
    
    a = 0
    b = 0
    c = 0
    d = 0
    for i in range(label.shape[0]):
        for j in range(i + 1, label.shape[0]):
            if(result_c[i] == result_c[j]):
                if(label[i] == label[j]):
                    a += 1
                else:
                    c += 1
            else:
                if(label[i] == label[j]):
                    b += 1
                else:
                    d += 1
    return float(a + d) / (a + b + c +d)

def draw(data, cluster_index):
    color = ["r", "g", "b"]
    for i, c in enumerate(cluster_index):
        for index in c:
            plt.scatter(data[index, 0], data[index, 1], color=color[i])
    plt.show()