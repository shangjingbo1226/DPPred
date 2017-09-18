#ifndef __UNION_FIND_SET__
#define __UNION_FIND_SET__

struct UnionFindSet
{
    vector<int> father;
    int componentNumber;
    
    UnionFindSet(int n) {
        father.resize(n);
        for (int i = 0; i < n; ++ i) {
            father[i] = i;
        }
        componentNumber = n;
    }
    
    int get(int x) {
        return father[x] == x ? x : father[x] = get(father[x]);
    }
    
    int size() {
        return componentNumber;
    }
    
    bool merge(int i, int j) {
        int fi = get(i), fj = get(j);
        if (fi != fj) {
            -- componentNumber;
            father[fi] = fj;
            return true;
        }
        return false;
    }
    
};

#endif
