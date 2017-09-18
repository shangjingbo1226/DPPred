#ifndef __RANDOM_FOREST_H__
#define __RANDOM_FOREST_H__

#include "../utils/helper.h"
#include "../utils/random.h"
using namespace RandomNumbers;
#include "../patterns/classification_rule.h"

namespace RandomForestRelated
{

vector<double> featureImportance;

int RANDOM_FEATURES = 4;
int RANDOM_POSITIONS = 8;

const int CLASSIFICATION = 0xa001;
const int REGRESSION = 0xa002;
const int SURVIVAL = 0xa003;

int TASK_TYPE = CLASSIFICATION; // default

double binaryEntropy(int p1, int total)
{
    if (p1 == 0 || p1 == total) {
        return 0;
    }
    double p = p1 / (double)total;
    return - p * log2(p) - (1 - p) * log2(1 - p);
}

double calculateLoss(const vector<int> IDs, const vector<double> &labels)
{
    if (TASK_TYPE == CLASSIFICATION) {
        // entropy
        unordered_map<int, int> hist;
        FOR (id, IDs) {
            ++ hist[(int)labels[*id]];
        }
        double entropy = 0;
        FOR (iter, hist) {
            double p = iter->second / (double) IDs.size();
            entropy += -p * log(p);
        }
        return entropy;
    } else if (TASK_TYPE == REGRESSION) {
        // mean square error
        double avg = 0;
        FOR (id, IDs) {
            avg += labels[*id];
        }
        if (IDs.size()) {
            avg /= IDs.size();
        }
        double squareError = 0;
        FOR (id, IDs) {
            squareError += sqr(avg - labels[*id]);
        }
        return squareError / IDs.size();
    } else if (TASK_TYPE == SURVIVAL) {
        cerr << "TODO survival" << endl;
        exit(-1);
    } else {
        myAssert(false, "Unknown Task Type!");
    }
    return 0;
}

struct TreeNode {
	bool leaf;
	int level, feature;
	double value, result;
	int left, right;

	TreeNode() {
		leaf = false;
		level = feature = left = right = -1;
		value = result = 0;
	}
};

class DecisionTree
{
public:
	vector<TreeNode> nodes;

	void dump(FILE* out) {
	    size_t size = nodes.size();
	    fwrite(&size, sizeof(size), 1, out);
	    if (size > 0) {
    	    fwrite(&nodes[0], sizeof(nodes[0]), size, out);
	    }
	}

	void load(FILE* in) {
	    size_t size;
	    fread(&size, sizeof(size), 1, in);
	    nodes.resize(size);
	    if (size > 0) {
    	    fread(&nodes[0], sizeof(nodes[0]), size, in);
	    }
    }

	DecisionTree() {}

	void train(const vector< vector<double> > &features, const vector<double> &results, int minNodeSize, int maxLevel = 18, vector<string> featureNames = vector<string>()) {
        int threadID = omp_get_thread_num();
	    if (features.size() == 0) {
	        return;
	    }

	    vector< vector<int> > featureGroups;
	    if (featureNames.size() != 0) {
	        unordered_map<string, int> name2id;
	        for (int i = 0; i < featureNames.size(); ++ i) {
	            string name = featureNames[i];
	            if (name.find("=") != -1) {
	                name = name.substr(0, name.find("="));
	            }
	            if (!name2id.count(name)) {
	                name2id[name] = featureGroups.size();
	                featureGroups.push_back(vector<int>());
	            }
	            featureGroups[name2id[name]].push_back(i);
	        }
	    } else {
	        for (int i = 0; i < features[0].size(); ++ i) {
	            featureGroups.push_back(vector<int>(1, i));
	        }
	    }

		TreeNode root;
		root.level = 0;
		nodes.push_back(root);

        // bootstrapping
		vector<int> rootBag;
		int samplesN = max((int)results.size(), 100);
		for (int i = 0; i < samplesN; ++ i) {
            rootBag.push_back(rng[threadID].next(results.size()));
		}

		vector<vector<int>> nodeBags;
		nodeBags.push_back(rootBag);

		for (int curNode = 0; curNode < (int)nodes.size(); ++ curNode) {
			TreeNode &node = nodes[curNode];
			vector<int> &bag = nodeBags[curNode];

			myAssert(bag.size() > 0, "[ERROR] empty node in decision tree!");
			myAssert(bag.size() >= minNodeSize, "[ERROR] bag is too small!");

			bool equal = true;

			double first = results[bag[0]];
			for (int i = 1; i < (int)bag.size(); ++ i) {
				if (sign(results[bag[i]] - first)) {
					equal = false;
					break;
				}
			}

			if (equal || (int)bag.size() < minNodeSize * 2 || node.level >= maxLevel) {
				// leaf
				node.leaf = true;
				for (int i = 0; i < (int)bag.size(); ++ i) {
					node.result += results[bag[i]];
				}
				node.result /= bag.size();
				continue;
			}

            double bagLoss = calculateLoss(bag, results);

			int bestFeature = -1;
			int bestLeft = 0, bestRight = 0;
			double bestValue = 0;
			double bestLoss = 1e100;
			vector<int> leftBag, rightBag;
			for (int _ = 0; _ < RANDOM_FEATURES; ++ _) {
			    int groupID = rng[threadID].next(featureGroups.size());
			    int featureID = featureGroups[groupID][rng[threadID].next(featureGroups[groupID].size())];

			    bool continuous = false;
			    if (featureGroups[groupID].size() == 1) {
			        // continuous variable
			        continuous = true;
			    } else {
			        // categorical variable
			        continuous = false;
                }

				for (int __ = 0; __ < RANDOM_POSITIONS; ++ __) {
				    double splitValue = 0.5; // for categorical variable
				    if (continuous) {
				        // continuous
    					int instanceID = bag[rng[threadID].next(bag.size())];
					    splitValue = features[instanceID][featureID];
				    } else {
				        // categorical
				        if (__) {
				            // get a new value
				            featureID = featureGroups[groupID][rng[threadID].next(featureGroups[groupID].size())];
				        }
				    }

					vector<int> currentLeftBag, currentRightBag;
					for (int i = 0; i < (int)bag.size(); ++ i) {
						int id = bag[i];
						if (features[id][featureID] < splitValue) {
                            currentLeftBag.push_back(id);
						} else {
                            currentRightBag.push_back(id);
						}
					}

					if (currentLeftBag.size() < minNodeSize || currentRightBag.size() < minNodeSize) {
						continue;
					}

                    double currentLoss = (calculateLoss(currentLeftBag, results) * currentLeftBag.size() + calculateLoss(currentRightBag, results) * currentRightBag.size()) / bag.size();

					if (currentLoss < bestLoss) {
						bestLoss = currentLoss;
						bestValue = splitValue;
						bestFeature = featureID;
						leftBag = currentLeftBag;
						rightBag = currentRightBag;
					}
				}
			}
			if (leftBag.size() < minNodeSize || rightBag.size() < minNodeSize) {
				// leaf
				node.leaf = true;
				for (int i = 0; i < (int)bag.size(); ++ i) {
					node.result += results[bag[i]];
				}
				node.result /= bag.size();
				continue;
			}

			myAssert(leftBag.size() >= minNodeSize && rightBag.size() >= minNodeSize, "[ERROR] bag is too small");

			featureImportance[bestFeature] += bagLoss - bestLoss;

			double nextValue = -1e100;
			for (int i = 0; i < (int)leftBag.size(); ++ i) {
				int id = leftBag[i];
                nextValue = max(nextValue, features[id][bestFeature]);
			}

			TreeNode left, right;

			left.level = right.level = node.level + 1;
			node.feature = bestFeature;
			node.value = (bestValue + nextValue) / 2;
			node.left = nodes.size();
			node.right = nodes.size() + 1;

			nodes.push_back(left);
			nodes.push_back(right);

			nodeBags.push_back(leftBag);
			nodeBags.push_back(rightBag);
		}
	}

	double estimate(vector<double> &features) {
		TreeNode *current = &nodes[0];
		while (!current->leaf) {
			if (features[current->feature] < current->value) {
				current = &nodes[current->left];
			} else {
				current = &nodes[current->right];
			}
		}
		return current->result;
	}

	void traverse(int id, Rule &current, Rules &all, vector< vector<double> > &train, vector<double> &trainY, int MIN_SUP) {
        if (current.satisfiedTrainings.size() < MIN_SUP) {
            return;
        }

        if (id != 0) {
            // not root
            current.loss = calculateLoss(current.satisfiedTrainings, trainY);
            all.push_back(current);
        }

	    if (nodes[id].leaf) {
	        return;
	    }

	    vector<int> bag = current.satisfiedTrainings;

        //split left & right
	    vector<int> leftBag, rightBag;
	    int index = nodes[id].feature;
	    double sep = nodes[id].value;
	    FOR (tid, bag) {
	        if (train[*tid][index] < sep) {
	            leftBag.push_back(*tid);
	        } else {
	            rightBag.push_back(*tid);
	        }
	    }

	    current.push_back(Condition(index, sep, true));
	    current.satisfiedTrainings = leftBag;
	    traverse(nodes[id].left, current, all, train, trainY, MIN_SUP);
	    current.pop_back();

	    current.push_back(Condition(index, sep, false));
	    current.satisfiedTrainings = rightBag;
	    traverse(nodes[id].right, current, all, train, trainY, MIN_SUP);
	    current.pop_back();

	    current.satisfiedTrainings = bag;
	}

	Rules getRules(vector< vector<double> > &train, vector<double> &trainY, int MIN_SUP) {
	    Rule current;
	    Rules all;
	    for (int i = 0; i < train.size(); ++ i) {
	        current.satisfiedTrainings.push_back(i);
	    }
	    traverse(0, current, all, train, trainY, MIN_SUP);
	    return all;
	}
};

class RandomForest
{
	vector<DecisionTree> trees;
	vector< vector<double> > features;
	vector<double> results;
public:
    void dump(string filename) {
        FILE* out = fopen(filename.c_str(), "wb");
        size_t size = trees.size();
        fwrite(&size, sizeof(size), 1, out);
        for (size_t i = 0; i < trees.size(); ++ i) {
            trees[i].dump(out);
        }
        fclose(out);
    }

    void load(string filename) {
        FILE* in = fopen(filename.c_str(), "rb");
        size_t size;
        fread(&size, sizeof(size), 1, in);
        trees.resize(size);
        for (size_t i = 0; i < trees.size(); ++ i) {
            trees[i].load(in);
        }
        fclose(in);
    }

	void clear() {
		features.clear();
		results.clear();
		trees.clear();
	}

	void train(vector< vector<double> > &_features, vector<double> _results, int treesNo = 100, int minNodeSize = 100, int maxLevel = 100, vector<string> featureNames = vector<string>()) {
		if (features.size() == 0) {
			features = _features;
			results = _results;
			if (features.size() > 0) {
    			featureImportance.resize(features[0].size(), 0);
			}
		}
		myAssert(features.size() == results.size(), "[ERROR] wrong training data!");
        trees.resize(treesNo);
        #pragma omp parallel for
		for (int i = 0; i < treesNo; ++ i) {
			trees[i].train(_features, _results, minNodeSize, maxLevel, featureNames);
		}
	}

	Rules getRules(vector< vector<double> > &train, vector<double> &trainY, int MIN_SUP) {
	    Rules ret;
		for (int i = 0; i < (int)trees.size(); ++ i) {
		    ret.extend(trees[i].getRules(train, trainY, MIN_SUP));
		}
		return ret;
	}

	double estimate(vector<double> &features) {
		if (trees.size() == 0) {
			return 0.0;
		}

		double sum = 0;
		for (int i = 0; i < (int)trees.size(); ++ i) {
			sum += trees[i].estimate(features);
		}
		return sum / trees.size();
	}
};
};

#endif
