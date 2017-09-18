#!/bin/sh

if [ "$#" -ne 2 ]; then
    echo "Usage: ./run.sh <dataset> <task_type: classification, regression>"
    exit 1
fi

#cd tools/lda/mallet-2.0.7
#make
#cd ../../..

dataset=$1
task_type=$2

TOPK=20
LOCAL_TOPK=10

MIN_SUP=10
MAX_DEPTH=6

RANDOM_FEATURES=4
RANDOM_POSITIONS=8

TREES=100

CLUSTERS=2

make

mkdir tmp
mkdir tmp/${dataset}

./bin/produce_candidate_patterns data/${dataset}/train.csv ${task_type} tmp/${dataset}/rule_set ${MIN_SUP} ${MAX_DEPTH} ${RANDOM_FEATURES} ${RANDOM_POSITIONS} ${TREES}
cd glmnet_matlab
../bin/select_patterns ../data/${dataset}/train.csv ../tmp/${dataset}/rule_set ${TOPK} ../tmp/${dataset}/top_${TOPK}_rules_set ${task_type}
cd ..

./bin/rebuild_features data/${dataset}/train.csv tmp/${dataset}/top_${TOPK}_rules_set tmp/${dataset}/train.top_${TOPK}.csv ${TOPK}
./bin/rebuild_features data/${dataset}/test.csv tmp/${dataset}/top_${TOPK}_rules_set tmp/${dataset}/test.top_${TOPK}.csv ${TOPK}

# python src/clustering/kmeans.py tmp/${dataset}/train.top_${TOPK} tmp/${dataset}/test.top_${TOPK} ${CLUSTERS} data/${dataset}/train data/${dataset}/test
python src/clustering/lda.py tmp/${dataset}/train.top_${TOPK} tmp/${dataset}/test.top_${TOPK} ${CLUSTERS} data/${dataset}/train data/${dataset}/test

CLUSTER_TEST_PREFIX=tmp/${dataset}/test.top_${TOPK}_cluster
CLUSTER_TRAIN_PREFIX=tmp/${dataset}/train.top_${TOPK}_cluster

NEW_TRAIN=tmp/${dataset}/train.top_${TOPK}_combined.csv
NEW_TEST=tmp/${dataset}/test.top_${TOPK}_combined.csv

rm ${NEW_TRAIN}
rm ${NEW_TEST}

touch ${NEW_TRAIN}
touch ${NEW_TEST}

for i in `seq 1 ${CLUSTERS}`;
do
	CLUSTER_TRAIN=${CLUSTER_TRAIN_PREFIX}_${i}.csv
	CLUSTER_TEST=${CLUSTER_TEST_PREFIX}_${i}.csv
	
	CLUSTER_RULE=tmp/${dataset}/rule_set_cluster_${i}
	CLUSTER_SELECT_RULE=tmp/${dataset}/top_${TOPK}_rules_set_cluster_${i}_local_top_${LOCAL_TOPK}
	
	./bin/produce_candidate_patterns ${CLUSTER_TRAIN} ${task_type} ${CLUSTER_RULE} ${MIN_SUP} ${MAX_DEPTH} ${RANDOM_FEATURES} ${RANDOM_POSITIONS} ${TREES}
	cd glmnet_matlab
	../bin/select_patterns ../${CLUSTER_TRAIN} ../${CLUSTER_RULE} ${LOCAL_TOPK} ../${CLUSTER_SELECT_RULE} ${task_type}
	cd ..
	
	./bin/combine_global_and_local ${CLUSTER_TRAIN} tmp/${dataset}/top_${TOPK}_rules_set ${CLUSTER_SELECT_RULE} ${NEW_TRAIN} ${TOPK}
	./bin/combine_global_and_local ${CLUSTER_TEST} tmp/${dataset}/top_${TOPK}_rules_set ${CLUSTER_SELECT_RULE} ${NEW_TEST} ${TOPK}
done

TRAIN_PRED=tmp/${dataset}/train_pred
TEST_PRED=tmp/${dataset}/test_pred

cd glmnet_matlab
../bin/predict ../${NEW_TRAIN} ../${NEW_TEST} ../${TEST_PRED} ../${TRAIN_PRED} ${task_type}
cd ..
