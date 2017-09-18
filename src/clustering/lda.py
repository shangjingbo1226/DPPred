from sklearn import cluster
from sklearn import preprocessing
import sys, os
import random

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

def to_mallet_format(data, savefile = './tmp/mallet_train'):
    with open(savefile, 'w') as writer:
        for doc in data:
            writer.write(" ".join([str(i) for i, value in enumerate(doc) if value != 0.0]) + "\n")

def assignment_file(filename, delim = '\t'):
    cluster_ids = []
    with open(filename) as source:
        source.readline()
        for line in source:
            entries = line.strip().split(delim)
            cluster_ids.append(int(entries[2]))

    return cluster_ids

def train_infer_lda(train_data, test_data, K, tmp_path = './tmp/'):

    n_iter = min(len(train_data), 1000)
    n_iter = max(500, n_iter)

    if not os.path.exists(tmp_path):
        os.makedirs(tmp_path)

    to_mallet_format(train_data, savefile = tmp_path + '/mallet_train')
    to_mallet_format(test_data, savefile = tmp_path + '/mallet_test')

    malletTrainFile = tmp_path + "train.mallet"
    malletTestFile = tmp_path + "test.mallet"
    modelFile = tmp_path + "infer.mallet"

    linePattern='"()()(.*)"'
    tokenPattern='"[0-9]+"'

    train_topic = tmp_path + "train_topic.txt"
    test_topic = tmp_path + "test_topic.txt"
    binary_path = "./tools/lda/mallet-2.0.7/binary/mallet"

    random.seed(19911226)
    random_seed = str(random.randint(1, 1000000000))

    print 'random_seed =', random_seed

    #import data
    os.system(binary_path + " import-file --input " + tmp_path + '/mallet_train' + " --output " +  malletTrainFile + " --line-regex " + linePattern + " --token-regex " + tokenPattern + " --keep-sequence")
    os.system(binary_path + " import-file --input " + tmp_path + '/mallet_test' + " --output " +  malletTestFile + " --line-regex " + linePattern + " --token-regex " + tokenPattern + " --keep-sequence")
    #train model and infer
    os.system(binary_path + " train-topics --random-seed " + random_seed +  " --input " +  malletTrainFile + " --num-topics " + str(K) + " --num-iterations " + str(n_iter) + " --output-topic-keys 10 --optimize-interval 100 --inferencer-filename " + modelFile + " --output-doc-topics " + train_topic)
    #os.system(binary_path + " infer-topics --random-seed " + random_seed +  " --input " +  malletTestFile + " --inferencer " + modelFile + " --output-doc-topics " + test_topic) 
    
    # read assignment
    #c_ids = assignment_file(train_topic, '\t') +  assignment_file(test_topic , ' ')
    c_ids = assignment_file(train_topic, '\t')
    return c_ids

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
    
    #clusterID = train_infer_lda(trainX, testX, clusterN)
    clusterID = train_infer_lda(trainX + testX, [], clusterN)
    
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