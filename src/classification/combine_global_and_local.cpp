#include "../utils/helper.h"
#include "../utils/union_find_set.h"
#include "../patterns/classification_rule.h"

#include "../utils/training_matrix.h"
using namespace TrainingMatrix;

int main(int argc, char* argv[])
{
    int topK;
	if (argc != 6 || sscanf(argv[5], "%d", &topK) != 1) {
        cerr << argc << endl;
        for (int i = 1; i < argc; ++ i) {
            cerr << argv[i] << endl;
        }
		fprintf(stderr, "[usage] <training/testing data, csv format> <selected rules> <local rules> <new training/testing data, csv format> <top-K>\n");
		return -1;
	}
	//cerr << "top-K = " << topK << endl;
	int dimension = loadFeatureMatrix(argv[1]);
	Rules global;
	global.load(argv[2]);
    global.resize(topK);

    Rules local;
    local.load(argv[3]);

    vector< vector<double> > X;
    vector<double> Y;
    int dimension2 = loadFeatureMatrix(argv[4], X, Y);
    cerr << "previous = " << dimension2 << endl;
    cerr << "current = " << local.size() << endl;

	FILE* out = tryOpen(argv[4], "w");

    fprintf(out, "label");
	if (dimension2 == 0) {
        for (int i = 0; i < global.size(); ++ i) {
            fprintf(out, ",global_%s", global[i].showup().c_str());
        }
    } else {
        for (int i = 0; i < featureNames.size(); ++ i) {
            fprintf(out, ",%s", featureNames[i].c_str());
        }
    }
    for (int i = 0; i < local.size(); ++ i) {
        fprintf(out, ",local_%s", local[i].showup().c_str());
    }
    fprintf(out, "\n");

    for (int i = 0; i < X.size(); ++ i) {
        if (fabs(Y[i] - (int)Y[i]) < EPS) {
            fprintf(out, "%d", (int)Y[i]);
        } else {
            fprintf(out, "%.10f", Y[i]);
        }
        for (int j = 0; j < X[i].size(); ++ j) {
            fprintf(out, ",%d", (int)X[i][j]);
        }
        for (int j = 0; j < local.size(); ++ j) {
            fprintf(out, ",0");
        }
        fprintf(out, "\n");
    }

	for (int i = 0; i < train.size(); ++ i) {
	    if (fabs(trainY[i] - (int)trainY[i]) < EPS) {
            fprintf(out, "%d", (int)trainY[i]);
        } else {
            fprintf(out, "%.10f", trainY[i]);
        }
	    for (int j = 0; j < global.size(); ++ j) {
            int sat = global[j].isSatisfiedBy(train[i]);
	        fprintf(out, ",%d", sat);
	    }
	    for (int j = global.size(); j < dimension2; ++ j) {
            fprintf(out, ",0");
	    }
        for (int j = 0; j < local.size(); ++ j) {
            int sat = local[j].isSatisfiedBy(train[i]);
	        fprintf(out, ",%d", sat);
	    }
	    fprintf(out, "\n");
	}
	fclose(out);

	return 0;
}

