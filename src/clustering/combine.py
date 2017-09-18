from sklearn import cluster
from sklearn import preprocessing
from math import sqrt
import sys

header = ''

def dump(file, id_file, id, x, y):
    file.write(str(y))
    for v in x:
        file.write(',' + str(v))
    file.write('\n')
    
    id_file.write(str(id) + '\n')

def loadFeatureMatrix(filename):
    global header
    X = []
    Y = []
    for line in open(filename):
        if line.find('label') == 0:
            header = line
            attributes = line.split(',')
            continue
        tokens = line.split(',')
        label = tokens[0]
        feature = []
        for i in xrange(len(tokens)):
            if attributes[i] == 'label':
                continue
            feature.append(float(tokens[i]))
        X.append(feature)
        Y.append(label)
    sys.stderr.write('loaded ' + str(len(X)) + ' X ' + str(len(X[0])) + '\n')
    return (X, Y)
    
def main(argv):
    if len(argv) != 5:
        print '[usage] <test prefix> <pred prefix> <cluster number> <combined_pred> <task type>'
        return
    TASK_TYPE = argv[4]
    clusterN = int(argv[2])
    
    pred_map = {}
    for id in xrange(clusterN):
        test_file =  argv[0] + '_' + str(id + 1) + '.csv'
        (testX, testY) = loadFeatureMatrix(test_file)
        test_id_file = argv[0] + '_' + str(id + 1) + '_id_file.csv'
        test_pred = argv[1] + '_' + str(id + 1)
        
        ids = open(test_id_file).readlines()
        preds = open(test_pred).readlines()
        
        for i in xrange(len(testY)):
            pred_map[int(ids[i])] = (preds[i].strip(), testY[i].strip())

    n = len(pred_map)
    print 'combined testing instances =', n
    combined = open(argv[3], 'w')
    rmse, accuracy = 0.0, 0.0
    for i in xrange(n):
        pred, label = pred_map[i]
        combined.write(pred + '\n')
        if TASK_TYPE == 'classification':
            accuracy += int(pred == label)
        elif TASK_TYPE == 'regression':
            rmse += (float(pred) - float(label)) * (float(pred) - float(label))
    combined.close()
    if TASK_TYPE == 'classification':
        print 'Combined Accuracy =', accuracy / n
    elif TASK_TYPE == 'regression':
        print 'Combined RMSE =', sqrt(rmse / n)
       

if __name__ == "__main__":
    main(sys.argv[1 : ])
