'''
@Description: 
@Author: Sqrti
@Date: 2020-06-25 23:35:13
@LastEditTime: 2020-06-26 13:18:24
@LastEditors: Sqrti
'''
import numpy as np

def PCA(data, th_type, th = 0.99, tar_dim = 2):
    eigen, eigen_v = np.linalg.eig(np.cov(data, rowvar = 0))
    eigen_v = eigen_v.T
    eigen_index = np.argsort(-eigen)
    all_eigen = np.sum(eigen)
    sum_m_eigen = 0.0
    dim = 0
    if (th_type == 0):
        for m in range (len(eigen)):
            if(sum_m_eigen / all_eigen < th < (sum_m_eigen + eigen[eigen_index[m]]) / all_eigen):
                break
            sum_m_eigen += eigen[eigen_index[m]]
        dim = m + 1
    else:
        dim = tar_dim
    return data.dot(eigen_v[eigen_index[:dim]].T)
