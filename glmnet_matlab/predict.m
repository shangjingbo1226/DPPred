function [ accuracy ] = predict( train_file, test_file, pred_file, train_pred_file, task )
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
train = load(train_file);
test = load(test_file);
[m, n] = size(train);

if strcmp(task, 'survival') == 1
	train_x = train(:, 1:n - 2);
	train_y = train(:, n-1:n);
	test_x = test(:, 1:n - 2);
	test_y = test(:, n-1:n);
else
	train_x = train(:, 1:n - 1);
	train_y = train(:, n);
	test_x = test(:, 1:n - 1);
	test_y = test(:, n);
end

options = glmnetSet;
options.alpha = 0;

if strcmp(task, 'classification') == 1
	fit = cvglmnet(train_x, train_y, 'multinomial', options);
		
	train_pred = glmnetPredict(fit.glmnet_fit, train_x, fit.lambda_min, 'class');
	dlmwrite(train_pred_file, train_pred, '\n');
	
	hit = 0;
	m = length(train_pred);
	for i = 1:m
		if train_pred(i) == train_y(i)
			hit = hit + 1;
		end
	end
	train_accuracy = hit / m

	pred = glmnetPredict(fit.glmnet_fit, test_x, fit.lambda_min, 'class');
	m = length(pred);
	dlmwrite(pred_file, pred, '\n');

	hit = 0;
	for i = 1:m
		if pred(i) == test_y(i)
			hit = hit + 1;
		end
	end
	accuracy = hit / m
	

	
	exit;
	
elseif strcmp(task, 'regression') == 1
	fit = cvglmnet(train_x, train_y, 'gaussian', options);

	train_pred = glmnetPredict(fit.glmnet_fit, train_x, fit.lambda_min, 'response');
	m = length(train_pred);
	train_error = train_pred - train_y;
	train_error = train_error' * train_error;
	train_error = train_error / m;
	train_error = sqrt(train_error)


	pred = glmnetPredict(fit.glmnet_fit, test_x, fit.lambda_min, 'response');
	
	m = length(pred);
	dlmwrite(pred_file, pred, '\n');
	
	error = pred - test_y;
	error = error' * error;
	error = error / m;
	error = sqrt(error)
	
	exit;

elseif strcmp(task, 'survival') == 1
	fit = cvglmnet(train_x, train_y, 'cox', options);
	pred = glmnetPredict(fit.glmnet_fit, test_x, fit.lambda_min, 'response');
	
	m = length(pred);
	dlmwrite(pred_file, pred, '\n');
	
	exit;
	
else
	display('Undefined Task!');
	exit;
end
	
end

