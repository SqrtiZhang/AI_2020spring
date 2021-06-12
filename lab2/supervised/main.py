'''
@Description: 
@Author: Sqrti
@Date: 2020-06-22 11:20:33
@LastEditTime: 2020-06-26 14:44:56
@LastEditors: Sqrti
'''
import argparse
import numpy as np
import pandas as pd
from sklearn.preprocessing import LabelEncoder
from sklearn.model_selection import train_test_split
from svm import SVM
from logistic import logisticregression
from knn import KNN
#from knn import predict
#import svm
import time

def loaddata(path, is_use_g = True, is_svm = True):
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
    dataset = pd.read_csv(path,sep=';')
    nominal = []

    for i, col in enumerate(dataset.columns):
        if(dataset[col].dtype == 'object'):
            nominal.append(i)
    #获取非数值类型的列

    dataset = dataset.to_numpy()
    #print(dataset.shape)
    if(is_svm):
        dataset[:,-1] = [1 if g3 >= 10 else -1 for g3 in dataset[:,-1]]
        dataset[:,-2] = [1 if g3 >= 10 else 0 for g3 in dataset[:,-2]]
        dataset[:,-3] = [1 if g3 >= 10 else 0 for g3 in dataset[:,-3]]
    else:
        dataset[:,-1] = [1 if g3 >= 10 else 0 for g3 in dataset[:,-1]]
        dataset[:,-2] = [1 if g3 >= 10 else 0 for g3 in dataset[:,-2]]
        dataset[:,-3] = [1 if g3 >= 10 else 0 for g3 in dataset[:,-3]]
    #将成绩改为二值
    
    encoder = LabelEncoder()
    for l in nominal:
        dataset[:,l] = encoder.fit_transform(dataset[:,l]) #编码为数值型
    
    if(is_use_g):
        return dataset[:,:-1], dataset[:,-1]
    else:
        return dataset[:,:-3], dataset[:,-1]


def norm(data, ifg1g2 = False):
    maxnum = np.max(data, axis=0)
    minnum = np.min(data, axis=0)
    if(ifg1g2):
      maxnum[-2] = 1
      maxnum[-1] = 1
      minnum[-2] = 0
      minnum[-1] = 0
    return (data - minnum)/(maxnum - minnum)

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
      default=r'data/student/student-mat.csv',
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
      help='0: SVM; 1: KNN; 2: LR'
    )

    parser.add_argument(
      '--kernel',
      type = int,
      default = 2,
      help='kernel for SVM. 0: Linear; 1: Polynomial; 2: RBF 3: Sigmoid'
    )
    parser.add_argument(
      '--C',
      type = int,
      default = 200,
      help='Soft margin penalty for svm'
    )
    parser.add_argument(
      '--d',
      type = int,
      default = 2,
      help='Parameter of polynomial kernel (x1x2+r)^d'
    )
    parser.add_argument(
      '--r',
      type = float,
      default = 1.0,
      help='Parameter of polynomial kernel (x1x2+r)^d'
    )
    parser.add_argument(
      '--gamma',
      type = float,
      default = 0.05,
      help='Parameter of rbf kernel'
    )
    parser.add_argument(
      '--epsilon',
      type = float,
      default = 0.0001,
      help='epsilon'
    )
    parser.add_argument(
      '--yita',
      type = float,
      default = 0.001,
      help='Parameter of sigmoid kernel  tanh(yita*x1x2+theta)'
    )
    parser.add_argument(
      '--theta',
      type = float,
      default = 0,
      help='Parameter of sigmoid kernel'
    )
    parser.add_argument(
      '--iter',
      type = int,
      default = 100,
      help='Iteration for LR'
    )
    parser.add_argument(
      '--learning_rate',
      type=float,
      default=0.01,
      help='Learning rate for LR.'
    )

    parser.add_argument(
      '--k',
      type = int,
      default = 9,
      help='Number of neighbors'
    )

    args = parser.parse_args()
    data, label = loaddata(args.data_dir, is_svm=(args.model_type == 0))
    data = norm(data)
    #data = reduce_dim(data)
    train_data, test_data, train_label, test_label = train_test_split(data, label, test_size = args.testing_percentage)
    #划分训练集与测试集
    start = time.time()
    if(args.model_type == 0):
      model = SVM(kernel=args.kernel, gamma=args.gamma, d=args.d, C=args.C, epsilon=args.epsilon, yita=args.yita, 
                 theta=args.theta, r=args.r, train_data=train_data, label = train_label)
      model.train()
      pred_label = model.predict(test_data)
      tmp = pred_label - test_label
      cac_f1(pred_label, test_label, 0)
      error = np.where(np.fabs(np.array(tmp,dtype=np.float))>1)[0]
      print(1-len(error)/np.float(len(pred_label)))
    elif(args.model_type == 1):
      pred_label = KNN(train_data, train_label, test_data, args.k, 0)
      #pred_label = predict(train_data, train_label, test_data)
      cac_f1(pred_label, test_label, 1)
      tmp = pred_label - test_label
      #print(tmp)
      #print(len(tmp))
      error = np.where(np.fabs(np.array(tmp,dtype=np.float))>0)[0]
      print(1-len(error)/np.float(len(pred_label)))
    elif(args.model_type == 2):
      model = logisticregression(3000, 0.05, 0.6)
      model.train(data, label)
      pred_label = model.predict(test_data)
      cac_f1(pred_label, test_label, 1)
    end = time.time()
    print(end-start)
main()