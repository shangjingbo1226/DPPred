#include "../classification/random_forest.h"
#include "../utils/helper.h"

using namespace RandomForestRelated;

#include "../utils/training_matrix.h"
using namespace TrainingMatrix;

int main(int argc, char* argv[])
{
    RANDOM_FEATURES = 4;
    RANDOM_POSITIONS = 8;
    int MIN_SUP = 5;
    int MAX_DEPTH = 6;
    int TREE_NOS = 100;
	if (argc < 6) {
		fprintf(stderr, "[usage] <training data, csv format> <task_type> <output rules> <min_sup> <depth> [optional, in order, # random features, # random positions, tree numbers]\n");
		return -1;
	}

	if (!strcmp(argv[2], "classification")) {
        TASK_TYPE = CLASSIFICATION;
	} else if (!strcmp(argv[2], "regression")) {
        TASK_TYPE = REGRESSION;
	} else if (!strcmp(argv[2], "survival")) {
        TASK_TYPE = SURVIVAL;
	} else {
        fprintf(stderr, "atsk_type should be one of classification, regression, and survival");
        return -1;
	}

    if (sscanf(argv[4], "%d", &MIN_SUP) != 1) {
        fprintf(stderr, "min_sup should be an integer!");
        return -1;
    }
    if (sscanf(argv[5], "%d", &MAX_DEPTH) != 1) {
        fprintf(stderr, "max_depth should be an integer!");
        return -1;
    }
    if (argc > 6 && sscanf(argv[6], "%d", &RANDOM_FEATURES) != 1) {
        fprintf(stderr, "# random features should be an integer!");
        return -1;
    }
    if (argc > 7 && sscanf(argv[7], "%d", &RANDOM_POSITIONS) != 1) {
        fprintf(stderr, "# random positions should be an integer!");
        return -1;
    }
	if (argc > 8 && sscanf(argv[8], "%d", &TREE_NOS) != 1) {
        fprintf(stderr, "tree numbers should be an integer!");
        return -1;
	}

    fprintf(stderr, "task_type = %s\n", argv[2]);
	fprintf(stderr, "MIN_SUP = %d, MAX_DEPTH = %d, TREE_NOS = %d\n", MIN_SUP, MAX_DEPTH, TREE_NOS);
	fprintf(stderr, "RANDOM_FEATURES = %d, RANDOM_POSIITONS = %d\n", RANDOM_FEATURES, RANDOM_POSITIONS);

	int dimension = loadFeatureMatrix(argv[1]);

	RandomForest *solver = new RandomForest();
    fprintf(stderr, "start to train...\n");

    RandomNumbers::initialize();

    solver->train(train, trainY, TREE_NOS, MIN_SUP, MAX_DEPTH, featureNames);

	Rules rules = solver->getRules(train, trainY, 1);
    int maximumConditions = 0, minimumConditions = dimension;
    double maxLoss = 0, minLoss = 1e100;
    for (int i = 0; i < rules.size(); ++ i) {
        maximumConditions = max(maximumConditions, rules[i].size());
        minimumConditions = min(minimumConditions, rules[i].size());
        minLoss = min(minLoss, rules[i].loss);
        maxLoss = max(maxLoss, rules[i].loss);
    }

    cout << "# Raw Rules = " << rules.size() << endl;
    cout << "# maximum conditions = " << maximumConditions << endl;
    cout << "# minimum conditions = " << minimumConditions << endl;
    cout << "max loss = " << maxLoss << ", min loss = " << minLoss << endl;

    rules.dump(argv[3]);

	return 0;
}
