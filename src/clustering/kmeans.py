from sklearn import cluster
from sklearn import preprocessing
import sys

header = ''

def dump(file, id_file, id, x, y):
    file.write(y)
    for v in x:
        file.write(',' + str(v))
    file.write('\n')
    
    id_file.write(str(id) + '\n')

def loadFeatureMatrix(filename):
    global header
    X = []
    Y = []
    for line in open(filename):
        if line.find('label') >= 0:
            header = 'label'
            attributes = line.strip().split(',')
            for attr in attributes:
                if attr != 'label':
                    header += ',' + attr
            header += '\n'

            continue
        tokens = line.strip().split(',')
        feature = []
        for i in xrange(len(tokens)):
            if attributes[i] == 'label':
                label = tokens[i]
                continue
            feature.append(float(tokens[i]))
        X.append(feature)
        Y.append(label)
    sys.stderr.write('loaded ' + str(len(X)) + ' X ' + str(len(X[0])) + '\n')
    return (X, Y)
    
def main(argv):
    if len(argv) != 5:
        print '[usage] <train prefix, without .csv> <test prefix, without .csv> <cluster number> <train feature file prefix> <test feature file prefix>'
        return
    (trainX, trainY) = loadFeatureMatrix(argv[0] + '.csv')
    (testX, testY) = loadFeatureMatrix(argv[1] + '.csv')
    (train_feature_X, train_feature_Y) = loadFeatureMatrix(argv[3] + '.csv')
    (test_feature_X, test_feature_Y) = loadFeatureMatrix(argv[4] + '.csv')
    
    clusterN = int(argv[2])

    #scaler = preprocessing.StandardScaler().fit(trainX)
    #trainX = scaler.transform(trainX)
    #testX = scaler.transform(testX)

    solver = cluster.KMeans(n_clusters = clusterN)
    solver.fit(trainX)
    
    allX = []
    allX.extend(trainX)
    allX.extend(testX)
    
    clusterID = solver.predict(allX)
    
    clusters = [[] for i in xrange(clusterN)]
    for (i, id) in enumerate(clusterID):
        clusters[id - 1].append(i)
        
    for id in xrange(clusterN):
        print 'Cluster', id, 'size =', len(clusters[id])
        
        train_file = open(argv[0] + '_cluster_' + str(id + 1) + '.csv', 'w')
        test_file =  open(argv[1] + '_cluster_' + str(id + 1) + '.csv', 'w')
        
        train_file.write(header)
        test_file.write(header)
        
        train_id_file =  open(argv[0] + '_cluster_' + str(id + 1) + '_id_file.csv', 'w')
        test_id_file =  open(argv[1] + '_cluster_' + str(id + 1) + '_id_file.csv', 'w')
        
        cnt_train, cnt_test = 0, 0
        for i in clusters[id]:
            if i < len(trainX):
                dump(train_file, train_id_file, i, train_feature_X[i], train_feature_Y[i])
                cnt_train += 1
            else:
                dump(test_file, test_id_file, i - len(trainX), test_feature_X[i - len(trainX)], test_feature_Y[i - len(trainX)])
                cnt_test += 1
        print '\ttrain =', cnt_train, ', test =', cnt_test
        
        train_file.close()
        test_file.close()
        
        
        

if __name__ == "__main__":
    main(sys.argv[1 : ])
