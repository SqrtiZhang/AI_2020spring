'''
@Description: 
@Author: Sqrti
@Date: 2020-06-24 19:13:07
@LastEditTime: 2020-06-25 21:42:05
@LastEditors: Sqrti
'''

import numpy as np
def cac_distance(x, y, dis_type=0):
    if(dis_type == 0):
        return np.sqrt(np.dot(x - y, x - y))
    else:
        return np.sum(np.fabs(x - y))

def KNN(train_data, label, test_data, k, type):
    distance = np.zeros((test_data.shape[0], train_data.shape[0]))
    for i in range(test_data.shape[0]):
        for j in range(train_data.shape[0]):
            distance[i, j] = cac_distance(test_data[i, :], train_data[j, :])

    sort_result = np.argsort(distance, axis=1)
    result = np.sum(train_data[sort_result[:,:k]][:,:,-1], axis=1)
    return np.array(result > k / 2, dtype=int)
 