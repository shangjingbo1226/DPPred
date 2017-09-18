# DPPred: An Effective Prediction Framework with Concise Discriminative Patterns

## Publications

Please cite the following two papers if you are using our tools. Thanks!

*   Jingbo Shang, Meng Jiang, Wenzhu Tong, Jinfeng Xiao, Jian Peng, Jiawei Han. "[DPPred: An Effective Prediction Framework with Concise Discriminative Patterns](https://arxiv.org/abs/1610.09778)", accepted by IEEE Transactions on Knowledge and Data Engineering, Sept. 2017. 

*   Jingbo Shang, Wenzhu Tong, Jian Peng, and Jiawei Han, "[DPClass: An Effective but Concise Discriminative Patterns-Based Classification Framework](http://hanj.cs.illinois.edu/pdf/sdm16_jshang-dpclass.pdf)", in Proc of 2016 SIAM Int. Conf. on Data Mining (SDM'16), Miami, FL, May 2016. [[code](https://github.com/shangjingbo1226/DPClass)] [[slides](http://shang7.web.engr.illinois.edu/slides/SDM16-DPClass.pdf)]

## Applications in Real-World Medical Dataset

*   Qian Cheng, Jingbo Shang, Joshua Juen, Jiawei Han and Bruce Schatz, "[Mining Discriminative Patterns to Predict Health Status for Cardiopulmonary Patients](http://hanj.cs.illinois.edu/pdf/bcb16_qcheng.pdf)", in Proc. of 2016 ACM Conf. on Bioinformatics, Computational Biology, and Health Informatics (ACM-BCB'16), Seattle, WA, Oct. 2016.

## New Features

Comparing to [DPClass](https://github.com/shangjingbo1226/DPClass), DPPred is now supporting two new features:

* Regression \& Multi-Class Classification.
* Local Discriminative Pattern Discovery after Clustering.

Please find more details in [this paper](https://arxiv.org/abs/1610.09778).

## Requirements

This tool mainly requires
```
g++-4.8 or higher
matlab
python
```

The python libs that we are using
```
sklearn
```

We developed and tested it on Ubuntu 16.04.

The current executables require OpenMP, which does not come by default on OS X. To be able to run it on OS X, follow [this stackoverflow post](http://stackoverflow.com/questions/20321988/error-enabling-openmp-ld-library-not-found-for-lgomp-and-clang-errors).

## Simple Run

You could execute the code in the following way:

```
./run.sh <dataset_name> <task_type>
```

Example:

```
./run.sh adult classification
./run.sh bike regression
```

## Local Patterns

If you are interested in local patterns, please use ```run_with_clustering.sh``` following the same format of ```run.sh```.

## Parameters

Overall, there are some parameters related to the pattern generation.

- TOPK (default = 20) is the number of (global) discriminative patterns that you want to use in the prediction. For regression tasks or high-dimensional datasets, we recommend a larger value like 30.
- MIN_SUP (default = 10) is the minimum number of training instances that should be contained in each leaf node in the random decision tree.
- MAX_DEPTH (default = 6) is the maximum depth of the random decision tree, which is also the maximum length of patterns.
- RANDOM_FEATURES (default = 4) is the number of random features will be tried for each node split in random decision trees.
- RANDOM_POSITIONS (default = 8) is the number of random values will be tried for each selected feature during node split in random decision trees.
- TREES (default = 100) is the number of trees.

If you are interested in local patterns, there are two more parameters:

- CLUSTERS (default = 2) is the number of clusters you want to further investigate.
- LOCAL_TOPK (default = 10) is the number of local discriminative patterns within each cluster.

We also provide K-Means as an alternative clustering method to the LDA.
