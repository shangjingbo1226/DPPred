#include "../utils/helper.h"
#include "classification_rule.h"
#include "../utils/training_matrix.h"
using namespace TrainingMatrix;

Rules selectRulesGLMNET(Rules &rules, int topK, vector<double> &labels, string task_type)
{
    if (rules.size() == 0 || topK == 0) {
        Rules selected;
        return selected;
    }
    vector<vector<int>> coef(labels.size(), vector<int>());
    for (int i = 0; i < rules.size(); ++ i) {
        FOR (id, rules[i].satisfiedTrainings) {
            coef[*id].push_back(i);
        }
    }
    FILE* out = tryOpen("../tmp/GLMNET.input", "w");
    for (int i = 0; i < labels.size(); ++ i) {
        FOR (rule_id, coef[i]) {
            fprintf(out, "%d %d %d\n", i + 1, (*rule_id) + 1, 1);
        }
        if (fabs(labels[i] - (int)labels[i]) < EPS) {
            fprintf(out, "%d %d %d\n", i + 1, rules.size() + 1, (int)labels[i]);
        } else {
            fprintf(out, "%d %d %.10f\n", i + 1, rules.size() + 1, labels[i]);
        }
    }
    fclose(out);
    string command = "matlab -nosplash -nodesktop -logfile remoteAutocode.log -r \"featureRanking('../tmp/GLMNET.input', '../tmp/GLMNET.output', '" + task_type + "')\"";
    system(command.c_str());

    Rules selected;
    FILE* in = tryOpen("../tmp/GLMNET.output", "r");
    getLine(in);
    vector<string> tokens = splitBy(line, ',');
    for (int i = 0, rule_id; i < tokens.size() && i < topK; ++ i) {
        fromString(tokens[i], rule_id);
        selected.push_back(rules[rule_id]);
    }
    return selected;
}

int main(int argc, char* argv[])
{
    int topK = 0;
	if (argc != 6 || sscanf(argv[3], "%d", &topK) != 1) {
		fprintf(stderr, "[usage] <training data, csv format> <candidate rules> <top-k rules> <output selected rules> <task_type>\n");
		return -1;
	}

	int dimension = loadFeatureMatrix(argv[1]);
	Rules rules;
	rules.load(argv[2]);

	if (topK > rules.size()) {
	    topK = rules.size();
	}

	cout << "start to select " << topK << " rules out of " << rules.size() << " rules..." << endl;

	rules.aggresively_unique();
	rules.sortByLoss();

	Rules selected = selectRulesGLMNET(rules, topK, trainY, argv[5]);

	cerr << "[Selected Top " << topK << " Rules]" << endl;
	for (int i = 0; i < selected.size(); ++ i) {
	    cerr << selected[i].loss << "\t" << selected[i].showup(featureNames) << endl;
	}
    selected.dump(argv[4]);

	return 0;
}

