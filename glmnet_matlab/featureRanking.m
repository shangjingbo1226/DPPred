function [ ranking ] = featureRanking( train_file, output_file, task)
values = load(train_file);
data = sparse(values(:, 1), values(:, 2), values(:, 3));
%data = values;
[m, n] = size(data)
if strcmp(task, 'survival') == 1
	x = data(:, 1:n-2);
	y = data(:, n-1:n);
	num_features = n - 2;
else
	x = data(:, 1:n-1);
	y = data(:, n);
	num_features = n - 1;
end

rank = zeros(1, num_features);
for i = 1:num_features
    rank(i) = n;
end

options = glmnetSet;
options.alpha = 1;

if strcmp(task, 'classification') == 1
	fit = glmnet(x, y, 'multinomial', options);
	weights = fit.beta;
	[m, num_class] = size(weights);
	[m, lambda] = size(weights{1})

	count = 0;
	for i = 1:lambda
		if count >= m
			break;
		end
		for j = 1:m
			nonzero = 0;
			for k = 1:num_class
				if abs(weights{k}(j, i)) > 0
					nonzero = 1;
					break;
				end
			end
			if nonzero == 1 && rank(j) == n
				%fit.lambda(i);
				rank(j) = count;
				count = count + 1;
			end            
		end
	end
	
elseif strcmp(task, 'regression') == 1
	fit = glmnet(x, y, 'gaussian', options);
	weights = fit.beta;
	[m, lambda] = size(weights)

	count = 0;
	for i = 1:lambda
		if count >= m
			break;
		end
		for j = 1:m
			if abs(weights(j, i)) > 0 && rank(j) == n
				%fit.lambda(i);
				rank(j) = count;
				count = count + 1;
			end            
		end
	end
	
elseif strcmp(task, 'survival') == 1
	fit = glmnet(x, y, 'cox', options);
else
	display('Undefined Task!');
	exit;
end


count
%pred = glmnetPredict(fit, x, fit.lambda(i - 1), 'link');
[tmp, ranking] = sort(rank);
csvwrite(output_file, ranking - 1);
exit;
end

