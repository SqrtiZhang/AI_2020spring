'''
@Description: 
@Author: Sqrti
@Date: 2020-06-25 23:10:29
@LastEditTime: 2020-06-26 13:14:03
@LastEditors: Sqrti
'''
import argparse
import numpy as np
import pandas as pd
from sklearn.preprocessing import LabelEncoder
from sklearn.model_selection import train_test_split
import time
from pca import PCA
import kmeans


def loaddata(path):
    '''
    @description: load the data
    @param: 
            path: data path
            is_use_g: whether to use G1 and G2 to train
            is_svm: choose label to be -1(SVM) or 0
    @return: 
            dataset[:,:-1]: attr
            dataset[:,-1]: label
    '''    
    dataset = pd.read_csv(path,sep=',', header=None)
    dataset = dataset.to_numpy()
    return dataset[:,1:], dataset[:,0]


def norm(data):
    maxnum = np.max(data, axis=0)
    minnum = np.min(data, axis=0)
    return (data - minnum)/(maxnum - minnum)

def norm_z_score(data):
    mean = data.mean(axis=0)
    std = data.std(axis=0)
    return (data - mean) / std

def reduce_dim(data):
  #return data[:, [1,6,7,13,14,15,16,17,20,21,22,23,26,27,29,30,31]]
  #return data[:,[0,1,2,6,7,13,16,29,30,31]]
  return data[:,[30,31]]

def cac_f1(pred, label, model_type):
  tp = np.sum(np.array(pred + label == 2, dtype=int))
  if(model_type == 0):
    tn = np.sum(np.array(pred + label == -2, dtype=int))
    fp = np.sum(np.array(pred - label == 2, dtype=int))
    fn = np.sum(np.array(pred - label == -2, dtype=int))
  else:
    tn = np.sum(np.array(pred + label == 0, dtype=int))
    fp = np.sum(np.array(pred - label == 1, dtype=int))
    fn = np.sum(np.array(pred - label == -1, dtype=int))
  p = tp * 1.0 / (tp + fp)
  r = tp * 1.0 / (tp + fn)
  print("F1:", 2 * p * r /(p + r))
  print("p", p)
  print("r",r)

#if __name__ == '__main__':
def main():
    parser = argparse.ArgumentParser()
    parser.add_argument(
      '--data_dir',
      type=str,
      default=r'data/wine/wine.data',
      help='Path to folders of data.'
    )
    parser.add_argument(
      '--testing_percentage',
      type = float,
      default = 0.3,
      help = 'What percentage of images to use as a test set.'
    )
    parser.add_argument(
      '--model_type',
      type = int,
      default = 0,
      help='0: PCA + Kmeans; 1: Kmeans'
    )
    parser.add_argument(
      '--th',
      type = float,
      default = 0.99,
      help = 'Threshold for pca.'
    )
    parser.add_argument(
      '--iter',
      type = int,
      default = 100,
      help='Iteration for kmeans'
    )
    parser.add_argument(
      '--k',
      type = int,
      default = 3,
      help = 'k for kmeans.'
    )
    args = parser.parse_args()
    data, label = loaddata(args.data_dir)
    data = norm_z_score(data)
    #data = norm(data)
    #data = reduce_dim(data)
    #train_data, test_data, train_label, test_label = train_test_split(data, label, test_size = args.testing_percentage)
    #划分训练集与测试集
    start = time.time()
    lower_data = PCA(data, th_type=0, th=0.99)
    print(lower_data.shape)

    cluster, si, cluster_index = kmeans.KMeans(lower_data, 3)
    print(si)
    ri = kmeans.cac_ri(cluster_index, label)
    print(ri)
    #print(lower_data.shape)
    end = time.time()
    draw_data = PCA(data, th_type=1, tar_dim=2)
    kmeans.draw(draw_data, cluster_index)
    print(end-start)
main()