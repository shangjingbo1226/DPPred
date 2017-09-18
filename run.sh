#!/bin/sh

if [ "$#" -ne 2 ]; then
    echo "Usage: ./run.sh <dataset> <task_type: classification, regression>"
    exit 1
fi

dataset=$1
task_type=$2

TOPK=20

MIN_SUP=10
MAX_DEPTH=6

RANDOM_FEATURES=4
RANDOM_POSITIONS=8

TREES=100

make

mkdir tmp
mkdir tmp/${dataset}

./bin/produce_candidate_patterns data/${dataset}/train.csv ${task_type} tmp/${dataset}/rule_set ${MIN_SUP} ${MAX_DEPTH} ${RANDOM_FEATURES} ${RANDOM_POSITIONS} ${TREES}
cd glmnet_matlab
../bin/select_patterns ../data/${dataset}/train.csv ../tmp/${dataset}/rule_set ${TOPK} ../tmp/${dataset}/top_${TOPK}_rules_set ${task_type}
cd ..

./bin/rebuild_features data/${dataset}/train.csv tmp/${dataset}/top_${TOPK}_rules_set tmp/${dataset}/train.top_${TOPK}.csv ${TOPK}
./bin/rebuild_features data/${dataset}/test.csv tmp/${dataset}/top_${TOPK}_rules_set tmp/${dataset}/test.top_${TOPK}.csv ${TOPK}

NEW_TRAIN=tmp/${dataset}/train.top_${TOPK}.csv
NEW_TEST=tmp/${dataset}/test.top_${TOPK}.csv

TRAIN_PRED=tmp/${dataset}/train_pred
TEST_PRED=tmp/${dataset}/test_pred

cd glmnet_matlab
../bin/predict ../${NEW_TRAIN} ../${NEW_TEST} ../${TEST_PRED} ../${TRAIN_PRED} ${task_type}
cd ..
