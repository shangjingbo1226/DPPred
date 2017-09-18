Use
	featureRanking(input_file, output_file) 
to generate the ranking list of features.

The input_file should have each instance as a row, where the last column is the label denoted by 0 or 1. The data is represented as sparse matrix.
The output_file will be a csv file of the ranking list of feature ids, starting from 0.

========================================================================

Use 
	predict(train_file, test_file, pred_file)
to predict the labels for test data

The train_file and test_file should have each instance as a row, where the last column is the label. The data is represented as dense matrix for the topk selected features.
The function first train a linear model on training data using all the topk features, and then predict labels on testing data.
The prediction results are saved to pred_file, where each line is 1 or 0 for each corresponding testing instance. Accuracy is output on screen.
