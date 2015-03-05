%
% create images from functions
%
num_cols = 100;
num_rows = 2;
img_mat = zeros(num_rows, num_cols);

% set up pattern handles
pattHandles; % will create a cell called patts
patterns = [53 54 10 29]; % same as Jaeger code
num_patterns = length(patterns);
collector = zeros(num_patterns, num_cols);
% for all patterns
for pattern_ix=1:num_patterns
	generator = patts{patterns(pattern_ix)};

	for i=1:num_cols
		inp = generator(i);
		collector(pattern_ix, i) = inp;
	end
end

% save series to file
img_mat = collector(1:2, :);
imwrite(mat2gray(img_mat, [-1 1]), 'pic_1.jpg');

img_mat = collector(3:4, :);
imwrite(mat2gray(img_mat, [-1 1]), 'pic_2.jpg');
