#include "../utils/helper.h"
#include "../utils/union_find_set.h"
#include "../patterns/classification_rule.h"

#include "../utils/training_matrix.h"
using namespace TrainingMatrix;

int main(int argc, char* argv[])
{
    int topK;
	if (argc != 5 || sscanf(argv[4], "%d", &topK) != 1) {
        cerr << argc << endl;
        for (int i = 1; i < argc; ++ i) {
            cerr << argv[i] << endl;
        }
		fprintf(stderr, "[usage] <training/testing data, csv format> <selected rules> <new training/testing data, csv format> <top-K>\n");
		return -1;
	}
	//cerr << "top-K = " << topK << endl;
	int dimension = loadFeatureMatrix(argv[1]);
	Rules rules;
	rules.load(argv[2]);

    rules.resize(topK);

	FILE* out = tryOpen(argv[3], "w");
	fprintf(out, "label");
	for (int i = 0; i < rules.size(); ++ i) {
	    fprintf(out, ",rule_%s", rules[i].showup().c_str());
	}
	fprintf(out, "\n");
	vector<int> sum(rules.size(), 0);
	for (int i = 0; i < train.size(); ++ i) {
        if (fabs(trainY[i] - (int)trainY[i]) < EPS) {
            fprintf(out, "%d", (int)trainY[i]);
        } else {
            fprintf(out, "%.10f", trainY[i]);
        }
	    for (int j = 0; j < rules.size(); ++ j) {
            int sat = rules[j].isSatisfiedBy(train[i]);
	        fprintf(out, ",%d", sat);
	        sum[j] += sat;
	    }
	    fprintf(out, "\n");
	}
	fclose(out);

    string filename = argv[1];
	if (filename.find("train") != -1) {
        bool flag = false;
        for (int i = 0; i < rules.size(); ++ i) {
            if (rules[i].satisfiedTrainings.size() != sum[i]) {
                fprintf(stderr, "[Warning] rule %d, %d v.s. %d\n", i, rules[i].satisfiedTrainings.size(), sum[i]);
                flag = true;
            }
        }
        if (!flag) {
            //fprintf(stderr, "Self-check passed!\n");
			;
        } else {
            fprintf(stderr, "Oooooooooooooooooops! You found some troubles!\n");
        }
    }

	return 0;
}

