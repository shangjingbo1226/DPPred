#ifndef __CLASSIFICATION_RULE__
#define __CLASSIFICATION_RULE__

#include "../utils/helper.h"

struct Condition
{
    int featureID;
    double separator;
    bool goLeft;

    Condition(int featureID = -1, double separator = 0.0, bool goLeft = true) : featureID(featureID), separator(separator), goLeft(goLeft) {
    }

    bool isSatisfiedBy(const vector<double> &features) {
        myAssert(0 <= featureID && featureID < (int)features.size(), "[ERROR] feature index error in rule!");
        return (features[featureID] < separator) == goLeft;
    }
};

bool byFeatureID(const Condition &a, const Condition &b)
{
    return a.featureID < b.featureID;
}

struct Rule
{
    vector<Condition> conditions;
    vector<int> satisfiedTrainings;
    double loss;

    void dump(FILE* out) {
        size_t size = conditions.size();
        fwrite(&size, sizeof(size), 1, out);
        if (size > 0) {
            fwrite(&conditions[0], size, sizeof(conditions[0]), out);
        }
        size = satisfiedTrainings.size();
        fwrite(&size, sizeof(size), 1, out);
        if (size > 0) {
            fwrite(&satisfiedTrainings[0], size, sizeof(satisfiedTrainings[0]), out);
        }
        fwrite(&loss, sizeof(loss), 1, out);
    }

    void load(FILE* in) {
        size_t size = 0;
        fread(&size, sizeof(size), 1, in);
        conditions.resize(size);
        if (size > 0) {
            fread(&conditions[0], size, sizeof(conditions[0]), in);
        }

        fread(&size, sizeof(size), 1, in);
        satisfiedTrainings.resize(size);
        if (size > 0) {
            fread(&satisfiedTrainings[0], size, sizeof(satisfiedTrainings[0]), in);
        }
        fread(&loss, sizeof(loss), 1, in);
    }

    void push_back(const Condition &condition) {
        conditions.push_back(condition);
    }

    void pop_back() {
        myAssert(conditions.size() > 0, "[ERROR] no enough rules to pop back");
        conditions.pop_back();
    }

    bool isSatisfiedBy(const vector<double> &features) {
        FOR (condition, conditions) {
            if (!condition->isSatisfiedBy(features)) {
                return false;
            }
        }
        return true;
    }

    int size() const {
        return conditions.size();
    }

    string showup(vector<string> &variableNames) {
        sort(conditions.begin(), conditions.end(), byFeatureID);
        string ret = "";
        for (int i = 0; i < conditions.size(); ++ i) {
            stringstream temp;
            temp << "(" << variableNames[conditions[i].featureID] << "[ID = " << conditions[i].featureID << "]" << (conditions[i].goLeft ? " < " : " >= ") << conditions[i].separator << ")";
            if (ret.size() > 0) {
                ret += " AND ";
            }
            ret += temp.str();
        }
        return ret;
    }

    void sortByFeatureID() {
        sort(conditions.begin(), conditions.end(), byFeatureID);
    }

    string showup() const {
        string ret = "";
        for (int i = 0; i < conditions.size(); ++ i) {
            stringstream temp;
            temp << "(" << conditions[i].featureID << (conditions[i].goLeft ? " < " : " >= ") << conditions[i].separator << ")";
            if (ret.size() > 0) {
                ret += " AND ";
            }
            ret += temp.str();
        }
        return ret;
    }
};

bool byLoss(const Rule &a, const Rule &b)
{
    return a.loss + EPS < b.loss || fabs(a.loss - b.loss) < EPS && a.satisfiedTrainings.size() < b.satisfiedTrainings.size()
            || fabs(a.loss - b.loss) < EPS && a.satisfiedTrainings.size() == b.satisfiedTrainings.size() && a.size() < b.size();
}

bool byString(const Rule &a, const Rule &b)
{
    return a.showup() < b.showup();
}

bool bySatisifiedTrainings(const Rule &a, const Rule &b)
{
    return a.satisfiedTrainings < b.satisfiedTrainings;
}

struct Rules
{
    vector<Rule> rules;

    void sortByLoss() {
        sort(rules.begin(), rules.end(), byLoss);
    }

    void aggresively_unique() {
        if (rules.size() > 1) {
            for (int i = 0; i < rules.size(); ++ i) {
                sort(rules[i].satisfiedTrainings.begin(), rules[i].satisfiedTrainings.end());
            }
            sort(rules.begin(), rules.end(), bySatisifiedTrainings);
            int ptr = 0;
            for (int i = 1; i < rules.size(); ++ i) {
                if (rules[i].satisfiedTrainings != rules[ptr].satisfiedTrainings) {
                    rules[++ ptr] = rules[i];
                }
            }
            rules.resize(ptr + 1);
        }
    }

    void unique() {
        if (rules.size() > 1) {
            for (int i = 0; i < rules.size(); ++ i) {
                rules[i].sortByFeatureID();
            }
            sort(rules.begin(), rules.end(), byString);
            int ptr = 0;
            for (int i = 1; i < rules.size(); ++ i) {
                if (rules[i].showup() != rules[ptr].showup()) {
                    rules[++ ptr] = rules[i];
                }
            }
            rules.resize(ptr + 1);
        }
    }

    void resize(int n) {
        if (n < rules.size()) {
            rules.resize(n);
        }
    }

    void load(string filename) {
        FILE* in = fopen(filename.c_str(), "rb");
        size_t size = 0;
        fread(&size, sizeof(size), 1, in);
        rules.resize(size);
        for (size_t i = 0; i < rules.size(); ++ i) {
            rules[i].load(in);
        }
        fclose(in);
    }

    void dump(string filename) {
        FILE* out = fopen(filename.c_str(), "wb");
        size_t size = rules.size();
        fwrite(&size, sizeof(size), 1, out);
        for (size_t i = 0; i < rules.size(); ++ i) {
            rules[i].dump(out);
        }
        fclose(out);
    }

    int size() const {
        return rules.size();
    }

    Rule operator [](int index) const {
        return rules[index];
    }

    Rule& operator [](int index) {
        return rules[index];
    }

    void push_back(const Rule &rule) {
        rules.push_back(rule);
    }

    void extend(const Rules &other) {
        for (int i = 0; i < other.size(); ++ i) {
            rules.push_back(other[i]);
        }
    }
};

#endif
