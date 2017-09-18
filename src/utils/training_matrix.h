#ifndef __TRAINING_MATRIX_H__
#define __TRAINING_MATRIX_H__

#include "helper.h"

namespace TrainingMatrix
{
vector< vector<double> > train;
vector<double> trainY;
vector<string> featureNames;

int loadFeatureMatrix(string filename)
{
	FILE* in = tryOpen(filename.c_str(), "r");
	getLine(in); // header
	vector<string> attributes = splitBy(line, ',');
	int dimension = 0;
	featureNames.clear();
	FOR (feat, attributes) {
	    if (*feat == "label") {
	        continue;
	    }
	    featureNames.push_back(*feat);
	    ++ dimension;
	}
	//fprintf(stderr, "# features = %d\n", dimension);
	for (;getLine(in);) {
		vector<string> tokens = splitBy(line, ',');
		vector<double> features(dimension, 0);
		int ptr = 0;
		double label;
		for (size_t i = 0; i < tokens.size(); ++ i) {
		    if (attributes[i] == "label") {
		        fromString(tokens[i], label);
		        continue;
		    }
			fromString(tokens[i], features[ptr ++]);
		}
		myAssert(ptr == dimension, "ptr exceeds the dimension");

        train.push_back(features);
        trainY.push_back(label);
	}
	fclose(in);
	//fprintf(stderr, "# training instances = %d\n", train.size());
    return dimension;
}

int loadFeatureMatrix(string filename, vector< vector<double> > &train, vector<double> &trainY)
{
	FILE* in = tryOpen(filename.c_str(), "r");
	getLine(in); // header
	vector<string> attributes = splitBy(line, ',');
	int dimension = 0;
	featureNames.clear();
	FOR (feat, attributes) {
	    if (*feat == "label") {
	        continue;
	    }
	    featureNames.push_back(*feat);
	    ++ dimension;
	}
	fprintf(stderr, "# features = %d\n", dimension);
	for (;getLine(in);) {
		vector<string> tokens = splitBy(line, ',');
		vector<double> features(dimension, 0);
		int ptr = 0;
		double label;
		for (size_t i = 0; i < tokens.size(); ++ i) {
		    if (attributes[i] == "label") {
		        fromString(tokens[i], label);
		        continue;
		    }
			fromString(tokens[i], features[ptr ++]);
		}
		myAssert(ptr == dimension, "ptr exceeds the dimension");

        train.push_back(features);
        trainY.push_back(label);
	}
	fclose(in);
	fprintf(stderr, "# training instances = %d\n", train.size());
    return dimension;
}

}

#endif
