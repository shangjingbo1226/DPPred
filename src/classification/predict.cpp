#include "../utils/helper.h"
#include "../utils/training_matrix.h"

vector< vector<double> > trainX, testX;
vector<double> trainY, testY;

int main(int argc, char* argv[])
{
	if (argc != 6) {
		fprintf(stderr, "[usage] <train.csv> <test.csv> <output: prediction> <task_type>\n");
		return -1;
	}

    int dimension = TrainingMatrix::loadFeatureMatrix(argv[1], trainX, trainY);
	int dimension2 = TrainingMatrix::loadFeatureMatrix(argv[2], testX, testY);

    FILE* out = tryOpen("../tmp/glmnet_train.txt", "w");
    for (int i = 0; i < trainX.size(); ++ i) {
        for (int j = 0; j < trainX[i].size(); ++ j) {
            fprintf(out, "%.6f ", trainX[i][j]);
        }
        if (fabs(trainY[i] - (int)trainY[i]) < EPS) {
            fprintf(out, "%d\n", (int)trainY[i]);
        } else {
            fprintf(out, "%.10f\n", trainY[i]);
        }
    }
    fclose(out);

    FILE* test_out = tryOpen("../tmp/glmnet_test.txt", "w");
    for (int i = 0; i < testX.size(); ++ i) {
        for (int j = 0; j < testX[i].size(); ++ j) {
            fprintf(test_out, "%.6f ", testX[i][j]);
        }
        if (fabs(testY[i] - (int)testY[i]) < EPS) {
            fprintf(test_out, "%d\n", (int)testY[i]);
        } else {
            fprintf(test_out, "%.10f\n", testY[i]);
        }
    }
    fclose(test_out);

    string command = "matlab -nosplash -nodesktop -logfile remoteAutocode.log -r \"predict('../tmp/glmnet_train.txt', '../tmp/glmnet_test.txt', '" + (string)argv[3] + "', '" + (string)argv[4] + "', '" + (string)argv[5] + "')\"";
    cerr << command << endl;
    system(command.c_str());

    if (false) {
        FILE* in = tryOpen(argv[3], "r");
        string filename = (string)argv[2] + ".prediction_glmnet";
        FILE* out = tryOpen(filename, "w");
        for (int i = 0; i < testY.size(); ++ i) {
            int x;
            myAssert(fscanf(in, "%d", &x) == 1 && 0 <= x && x <= 1, "[ERROR] prediction file error");
            fprintf(out, "%d\n", x == testY[i]);
        }
        fclose(in);
        fclose(out);
    }

	return 0;
}

